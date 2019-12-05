/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#include "TOARange.h"

static asn_TYPE_member_t asn_MBR_TOARange_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct TOARange, computation_time),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Timesec,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"computation-time"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TOARange, eta),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ETA,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"eta"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TOARange, latitudeLongitude),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ADSv2LatitudeLongitude,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"latitudeLongitude"
		},
	{ ATF_POINTER, 1, offsetof(struct TOARange, name),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_WaypointName,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"name"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TOARange, windErrorModelUsed),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_WindErrorModelUsed,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"windErrorModelUsed"
		},
	{ ATF_POINTER, 2, offsetof(struct TOARange, earliest),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ETA,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"earliest"
		},
	{ ATF_POINTER, 1, offsetof(struct TOARange, latest),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ETA,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"latest"
		},
};
static const int asn_MAP_TOARange_oms_1[] = { 3, 5, 6 };
static const ber_tlv_tag_t asn_DEF_TOARange_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_TOARange_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* computation-time */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* eta */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* latitudeLongitude */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* name */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* windErrorModelUsed */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* earliest */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* latest */
};
static asn_SEQUENCE_specifics_t asn_SPC_TOARange_specs_1 = {
	sizeof(struct TOARange),
	offsetof(struct TOARange, _asn_ctx),
	asn_MAP_TOARange_tag2el_1,
	7,	/* Count of tags in the map */
	asn_MAP_TOARange_oms_1,	/* Optional members */
	3, 0,	/* Root/Additions */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_TOARange = {
	"TOARange",
	"TOARange",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_TOARange_tags_1,
	sizeof(asn_DEF_TOARange_tags_1)
		/sizeof(asn_DEF_TOARange_tags_1[0]), /* 1 */
	asn_DEF_TOARange_tags_1,	/* Same as above */
	sizeof(asn_DEF_TOARange_tags_1)
		/sizeof(asn_DEF_TOARange_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_TOARange_1,
	7,	/* Elements count */
	&asn_SPC_TOARange_specs_1	/* Additional specs */
};

