/*
 *  This file is a part of dumpvdl2
 *
 *  Copyright (c) 2017-2023 Tomasz Lemiech <szpajder@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Offset-based reassembly engine. Currently only CLNP needs this.
 * The algo and data structures are different enough from sequence-based
 * reassembly implementation in libacars, that it justifies keeping them
 * separate at the expense of a slight code duplication.
 */

#include <sys/time.h>                   // struct timeval
#include <string.h>                     // strdup
#include <libacars/hash.h>              // la_hash
#include <libacars/list.h>              // la_list
#include "dumpvdl2.h"                   // NEW, XCALLOC
#include "reassembly.h"

typedef struct reasm_table_s {
	void const *key;                    /* a pointer identifying the protocol
	                                       owning this reasm_table (type_descriptor
	                                       can be used for this purpose). Due to small
	                                       number of protocols, la_hash would be an overkill
	                                       here. */
	la_hash *fragment_table;            /* keyed with packet identifiers, values are
	                                       reasm_table_entries */
	reasm_table_funcs funcs;            /* protocol-specific callbacks */
	int cleanup_interval;               /* expire old entries every cleanup_interval
	                                       number of processed fragments */
	int frag_cnt;                       /* counts added fragments (up to cleanup_interval) */
} reasm_table;

struct reasm_ctx_s {
	la_list *rtables;                   /* list of reasm_tables, one per protocol */
};

// the header of the fragment list
typedef struct {
	int frags_collected_total_len;      /* sum of fragment_data_len for all fragments received */

	int total_pdu_len;                  /* total length of the reassembled message
	                                       (copied from reasm_fragment_info of the 1st fragment) */

	struct timeval first_frag_rx_time;  /* time of arrival of the first fragment */

	struct timeval reasm_timeout;       /* reassembly timeout to be applied to this message */

	la_list *fragment_list;             /* payloads of all fragments gathered so far */
	bool have_first_fragment;           /* whether we've already collected the first fragment of this PDU */
} reasm_table_entry;

// fragment list entry
struct fragment {
	int start;
	int end;
	octet_string_t *data;
};

reasm_ctx *reasm_ctx_new() {
	NEW(reasm_ctx, rctx);
	return rctx;
}

static void fragment_destroy(void *data) {
	if(data == NULL) {
		return;
	}
	struct fragment *f = data;
	octet_string_destroy(f->data);
	XFREE(f);
}

static void reasm_table_entry_destroy(void *rt_ptr) {
	if(rt_ptr == NULL) {
		return;
	}
	reasm_table_entry *rt_entry = rt_ptr;
	la_list_free_full(rt_entry->fragment_list, fragment_destroy);
	XFREE(rt_entry);
}

static void reasm_table_destroy(void *table) {
	if(table == NULL) {
		return;
	}
	reasm_table *rtable = table;
	la_hash_destroy(rtable->fragment_table);
	XFREE(rtable);
}

void reasm_ctx_destroy(void *ctx) {
	if(ctx == NULL) {
		return;
	}
	reasm_ctx *rctx = ctx;
	la_list_free_full(rctx->rtables, reasm_table_destroy);
	XFREE(rctx);
}

reasm_table *reasm_table_lookup(reasm_ctx *rctx, void const *table_id) {
	ASSERT(rctx != NULL);
	ASSERT(table_id != NULL);

	for(la_list *l = rctx->rtables; l != NULL; l = la_list_next(l)) {
		reasm_table *rt = l->data;
		if(rt->key == table_id) {
			return rt;
		}
	}
	return NULL;
}

#define REASM_DEFAULT_CLEANUP_INTERVAL 100

reasm_table *reasm_table_new(reasm_ctx *rctx, void const *table_id,
		reasm_table_funcs funcs, int cleanup_interval) {
	ASSERT(rctx != NULL);
	ASSERT(table_id != NULL);
	ASSERT(funcs.get_key);
	ASSERT(funcs.get_tmp_key);
	ASSERT(funcs.hash_key);
	ASSERT(funcs.compare_keys);
	ASSERT(funcs.destroy_key);

	reasm_table *rtable = reasm_table_lookup(rctx, table_id);
	if(rtable != NULL) {
		goto end;
	}
	rtable = XCALLOC(1, sizeof(reasm_table));
	rtable->key = table_id;
	rtable->fragment_table = la_hash_new(funcs.hash_key, funcs.compare_keys,
			funcs.destroy_key, reasm_table_entry_destroy);
	rtable->funcs = funcs;

	// Replace insane values with reasonable default
	rtable->cleanup_interval = cleanup_interval > 0 ?
		cleanup_interval : REASM_DEFAULT_CLEANUP_INTERVAL;
	rctx->rtables = la_list_append(rctx->rtables, rtable);
end:
	return rtable;
}

// Checks if time difference between rx_first and rx_last is greater than timeout.
static bool reasm_timed_out(struct timeval rx_last, struct timeval rx_first,
		struct timeval timeout) {
	if(timeout.tv_sec == 0 && timeout.tv_usec == 0) {
		return false;
	}
	struct timeval to = {
		.tv_sec = rx_first.tv_sec + timeout.tv_sec,
		.tv_usec = rx_first.tv_usec + timeout.tv_usec
	};
	if(to.tv_usec > 1e9) {
		to.tv_sec++;
		to.tv_usec -= 1e9;
	}
	debug_print(D_MISC, "rx_first: %lu.%lu to: %lu.%lu rx_last: %lu.%lu\n",
			rx_first.tv_sec, rx_first.tv_usec, to.tv_sec, to.tv_usec, rx_last.tv_sec, rx_last.tv_usec);
	return (rx_last.tv_sec > to.tv_sec ||
			(rx_last.tv_sec == to.tv_sec && rx_last.tv_usec > to.tv_usec));
}

// Simple check for equality of fragments.
// Only offsets and lengths are compared. No data comparison is done
// to speed things up.
static bool fragments_are_equal(struct fragment const *f1, struct fragment const *f2) {
		return f1->start == f2->start && f1->end == f2->end;
}

// Check if fragments overlap
static bool fragments_overlap(struct fragment const *f1, struct fragment const *f2) {
	return (f2->start <= f1->start && f1->start <= f2->end) ||
			(f1->start <= f2->start && f2->start <= f1->end);
}

// Callback for la_hash_foreach_remove used during reassembly table cleanups.
static bool is_rt_entry_expired(void const *keyptr, void const *valptr, void *ctx) {
	UNUSED(keyptr);
	ASSERT(valptr != NULL);
	ASSERT(ctx != NULL);

	reasm_table_entry const *rt_entry = valptr;
	struct timeval *now = ctx;
	return reasm_timed_out(*now, rt_entry->first_frag_rx_time, rt_entry->reasm_timeout);
}

// Removes expired entries from the given reassembly table.
static void reasm_table_cleanup(reasm_table *rtable, struct timeval now) {
	ASSERT(rtable != NULL);
	ASSERT(rtable->fragment_table != NULL);
	int deleted_count = la_hash_foreach_remove(rtable->fragment_table,
			is_rt_entry_expired, &now);
	// Avoid compiler warning when DEBUG is off
	UNUSED(deleted_count);
	debug_print(D_MISC, "Expired %d entries\n", deleted_count);
}

// Core reassembly logic.
// Validates the given message fragment and appends it to the reassembly table
// fragment la_list.
reasm_status reasm_fragment_add(reasm_table *rtable, reasm_fragment_info const *finfo) {
	ASSERT(rtable != NULL);
	ASSERT(finfo != NULL);
	if(finfo->pdu_info == NULL || finfo->offset < 0) {
		return REASM_ARGS_INVALID;
	}
	if(finfo->fragment_data == NULL || finfo->fragment_data_len < 1) {
		return REASM_ARGS_INVALID;
	}

	// Skip non-fragmented packets without further processing.
	if(finfo->is_final_fragment && finfo->offset == 0) {
		return REASM_SKIPPED;
	}

	// Don't allow zero timeout. This would prevent stale rt_entries from being
	// expired, causing a massive memory leak.
	if(finfo->reasm_timeout.tv_sec == 0 && finfo->reasm_timeout.tv_usec == 0) {
		return REASM_ARGS_INVALID;
	}

	int frag_end = finfo->offset + finfo->fragment_data_len - 1;
	reasm_status ret = REASM_UNKNOWN;
	void *lookup_key = rtable->funcs.get_tmp_key(finfo->pdu_info);
	ASSERT(lookup_key != NULL);
	reasm_table_entry *rt_entry = NULL;
restart:
	rt_entry = la_hash_lookup(rtable->fragment_table, lookup_key);
	if(rt_entry == NULL) {
		rt_entry = XCALLOC(1, sizeof(reasm_table_entry));
		rt_entry->first_frag_rx_time = finfo->rx_time;
		rt_entry->reasm_timeout = finfo->reasm_timeout;
		rt_entry->total_pdu_len = 0;        // to be computed later
		rt_entry->frags_collected_total_len = 0;
		debug_print(D_MISC, "Adding new rt_table entry (rx_time: %lu.%lu timeout: %lu.%lu)\n",
				rt_entry->first_frag_rx_time.tv_sec, rt_entry->first_frag_rx_time.tv_usec,
				rt_entry->reasm_timeout.tv_sec, rt_entry->reasm_timeout.tv_usec);
		void *msg_key = rtable->funcs.get_key(finfo->pdu_info);
		ASSERT(msg_key != NULL);
		la_hash_insert(rtable->fragment_table, msg_key, rt_entry);
	}

	// Check reassembly timeout
	if(reasm_timed_out(finfo->rx_time, rt_entry->first_frag_rx_time, rt_entry->reasm_timeout) == true) {

		// If reassembly timeout has expired, we treat this fragment as a part of
		// a new message. Remove the old rt_entry and create new one.

		debug_print(D_MISC, "reasm timeout expired; creating new rt_entry\n");
		la_hash_remove(rtable->fragment_table, lookup_key);
		goto restart;
	}

	// Create new fragment list entry.
	// Don't copy the data initially - this will be done when all sanity checks
	// are completed.
	NEW(struct fragment, current_fragment);
	current_fragment->start = finfo->offset;
	current_fragment->end = frag_end;

	// Compare the current fragment with fragments seen so far.
	// Check for duplicates and overlaps.
	for(la_list *l = rt_entry->fragment_list; l != NULL; l = la_list_next(l)) {
		struct fragment *f = l->data;
		if(fragments_are_equal(current_fragment, f)) {
			ret = REASM_DUPLICATE;
			goto cleanup;
		} else if(fragments_overlap(current_fragment, f)) {
			debug_print(D_MISC, "fragment overlap detected (current: start=%d end=%d existing: start=%d end=%d)\n",
					current_fragment->start, current_fragment->end, f->start, f->end);
			ret = REASM_OVERLAP;
			goto cleanup;
		}
	}

	// If this is the final fragment of this PDU, then compute the
	// total PDU length (provided that it has not been computed earlier)

	if(finfo->is_final_fragment) {
		if(rt_entry->total_pdu_len < 1) {
			rt_entry->total_pdu_len = finfo->offset + finfo->fragment_data_len;
			debug_print(D_MISC, "Final fragment: offset %d fragment_data_len %d -> total_pdu_len %d\n",
					finfo->offset, finfo->fragment_data_len, rt_entry->total_pdu_len);
		} else {
			debug_print(D_MISC, "Multiple final fragments in this PDU? Discarding.\n");
			ret = REASM_BOGUS_FINAL_FRAGMENT;
			goto cleanup;
		}
	}

	// All checks succeeded. Add the fragment to the list

	debug_print(D_MISC, "Good fragment (start=%d end=%d), adding to the list\n",
			current_fragment->start, current_fragment->end);

	uint8_t *fragment_data = XCALLOC(finfo->fragment_data_len, sizeof(uint8_t));
	memcpy(fragment_data, finfo->fragment_data, finfo->fragment_data_len);
	current_fragment->data = octet_string_new(fragment_data, finfo->fragment_data_len);
	rt_entry->fragment_list = la_list_append(rt_entry->fragment_list, current_fragment);
	rt_entry->frags_collected_total_len += finfo->fragment_data_len;

	// Reassembly is complete if total_pdu_len for this rt_entry is set
	// and we've already collected the required amount of data.

	if(rt_entry->total_pdu_len < 1 || rt_entry->frags_collected_total_len < rt_entry->total_pdu_len) {
		ret = REASM_IN_PROGRESS;
	} else if(rt_entry->frags_collected_total_len == rt_entry->total_pdu_len) {
		ret = REASM_COMPLETE;
	} else {
		// We've collected more data than needed?
		// This really shouldn't happen.
		debug_print(D_MISC, "Bad length: too much data collected from fragments (%d > pdu_len %d\n",
				rt_entry->frags_collected_total_len, rt_entry->total_pdu_len);
		ret = REASM_BAD_LEN;
	}
	goto end;

cleanup:

	XFREE(current_fragment);

end:

	// Update fragment counter and expire old entries if necessary.
	// Expiration is performed in relation to rx_time of the fragment currently
	// being processed. This allows processing historical data with timestamps in
	// the past.

	if(++rtable->frag_cnt > rtable->cleanup_interval) {
		reasm_table_cleanup(rtable, finfo->rx_time);
		rtable->frag_cnt = 0;
	}
	debug_print(D_MISC, "Result: %d\n", ret);
	XFREE(lookup_key);
	return ret;
}

// Returns the reassembled payload and removes the packet data from reassembly table
int reasm_payload_get(reasm_table *rtable, void const *pdu_info, uint8_t **result) {
	ASSERT(rtable != NULL);
	ASSERT(pdu_info != NULL);
	ASSERT(result != NULL);

	void *tmp_key = rtable->funcs.get_tmp_key(pdu_info);
	ASSERT(tmp_key);

	size_t result_len = -1;
	reasm_table_entry *rt_entry = la_hash_lookup(rtable->fragment_table, tmp_key);
	if(rt_entry == NULL) {
		result_len = -1;
		goto end;
	}
	if(rt_entry->frags_collected_total_len < 1) {
		result_len = 0;
		goto end;
	}
	// Reassemble all fragments in the correct order.
	// Append a NULL byte at the end of the reassembled buffer, so that the caller
	// may cast it to char * in case this is a text message.
	uint8_t *reasm_buf = XCALLOC(rt_entry->frags_collected_total_len + 1, sizeof(uint8_t));
	uint8_t *ptr = reasm_buf;
	int wanted_offset = 0;
	bool done = false;
	bool fragment_found = false;
	do {
		fragment_found = false;
		for(la_list *l = rt_entry->fragment_list; l != NULL; l = la_list_next(l)) {
			struct fragment *f = l->data;
			if(f->start == wanted_offset) {
				memcpy(ptr, f->data->buf, f->data->len);
				ptr += f->data->len;
				fragment_found = true;
				if(f->end + 1 == rt_entry->frags_collected_total_len) {
					done = true;
				} else {
					wanted_offset = f->end + 1;
				}
			}
		}
	} while(fragment_found && !done);

	ASSERT(fragment_found);     // This shouldn't happen - we should have exited the above loop with done == true
	reasm_buf[rt_entry->frags_collected_total_len] = '\0'; // buffer len is frags_collected_total_len + 1
	*result = reasm_buf;
	result_len = rt_entry->frags_collected_total_len;
	la_hash_remove(rtable->fragment_table, tmp_key);
end:
	XFREE(tmp_key);
	return result_len;
}

char const *reasm_status_name_get(reasm_status status) {
	static char const *reasm_status_names[] = {
		[REASM_UNKNOWN] = "unknown",
		[REASM_COMPLETE] = "complete",
		[REASM_IN_PROGRESS] = "in progress",
		[REASM_SKIPPED] = "skipped",
		[REASM_DUPLICATE] = "duplicate",
		[REASM_BAD_LEN] = "bad length",
		[REASM_OVERLAP] = "overlapped fragment",
		[REASM_BOGUS_FINAL_FRAGMENT] = "bogus final fragment",
		[REASM_ARGS_INVALID] = "invalid args"
	};
	if(status < 0 || status > REASM_STATUS_MAX) {
		return NULL;
	}
	return reasm_status_names[status];
}
