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
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>               // struct timeval
#include <libacars/libacars.h>      // la_proto_node
#include <libacars/list.h>          // la_list
#include <libacars/reassembly.h>    // la_reasm_ctx
#include "config.h"
#include "x25.h"
#include "reassembly.h"             // reasm_status

#define CLNP_NPDU_DT  0x1c
#define CLNP_NDPU_MD  0x1d
#define CLNP_NDPU_ER  0x01
#define CLNP_NDPU_ERP 0x1e
#define CLNP_NDPU_ERQ 0x1f

#define CLNP_MIN_LEN 9          // length of the fixed part of the header
typedef struct {
	uint8_t pid;
	uint8_t len;
	uint8_t version;
	uint8_t lifetime;
#ifdef IS_BIG_ENDIAN
	uint8_t sp:1;
	uint8_t ms:1;
	uint8_t er:1;
	uint8_t type:5;
#else
	uint8_t type:5;
	uint8_t er:1;
	uint8_t ms:1;
	uint8_t sp:1;
#endif
	uint8_t seg_len[2];         // not using uint16_t to avoid padding and to match PDU octet layout
	uint8_t cksum[2];
} clnp_hdr_t;

typedef struct {
	uint8_t *reasm_buf;
	reasm_status reasm_status;
	// fixed part
	clnp_hdr_t *hdr;
	// options
	la_list *options;
	// address part
	octet_string_t src_nsap, dst_nsap;
	// decoded fields from fixed part
	struct timeval lifetime;
	uint16_t seg_len;
	uint16_t cksum;
	// segmentation part
	uint16_t pdu_id, offset, total_pdu_len;
	// error flags
	bool err;
} clnp_pdu_t;

#define CLNP_COMPRESSED_MIN_LEN 4
typedef struct {
#ifdef IS_BIG_ENDIAN
	uint8_t type:4;
	uint8_t priority:4;
#else
	uint8_t priority:4;
	uint8_t type:4;
#endif
	uint8_t lifetime;
	union {
		uint8_t val;
		struct {
#ifdef IS_BIG_ENDIAN
			uint8_t p:1;
			uint8_t q:1;
			uint8_t r:1;
			uint8_t st:1;
			uint8_t ce:1;
			uint8_t tc:1;
			uint8_t et:1;
			uint8_t ec:1;
#else
			uint8_t ec:1;
			uint8_t et:1;
			uint8_t tc:1;
			uint8_t ce:1;
			uint8_t st:1;
			uint8_t r:1;
			uint8_t q:1;
			uint8_t p:1;
#endif
		} fields;
	} flags;
#ifdef IS_BIG_ENDIAN
	uint8_t exp:1;
	uint8_t lref_a:7;
#else
	uint8_t lref_a:7;
	uint8_t exp:1;
#endif
} clnp_compressed_data_pdu_hdr_t;

typedef struct {
	clnp_compressed_data_pdu_hdr_t *hdr;
	uint8_t *reasm_buf;
	reasm_status reasm_status;
	struct timeval lifetime;
	uint16_t lref;
	uint16_t pdu_id, offset, total_pdu_len;
	bool derived;
	bool is_segmentation_permitted;
	bool more_segments;
	bool err;
} clnp_compressed_data_pdu_t;

// clnp.c
la_proto_node *clnp_pdu_parse(uint8_t *buf, uint32_t len, uint32_t *msg_type,
		reasm_contexts *rtables, struct timeval rx_time, uint32_t src_addr, uint32_t dst_addr);
la_proto_node *clnp_compressed_data_pdu_parse(uint8_t *buf, uint32_t len, uint32_t *msg_type,
		reasm_contexts *rtables, struct timeval rx_time, uint32_t src_addr, uint32_t dst_addr);
