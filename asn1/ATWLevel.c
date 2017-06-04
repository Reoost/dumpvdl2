/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "PMCPDLCMessageSetVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#include "ATWLevel.h"

static asn_TYPE_member_t asn_MBR_ATWLevel_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ATWLevel, atw),
		(ASN_TAG_CLASS_UNIVERSAL | (10 << 2)),
		0,
		&asn_DEF_ATWLevelTolerance,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"atw"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ATWLevel, level),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_Level,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"level"
		},
};
static const ber_tlv_tag_t asn_DEF_ATWLevel_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_ATWLevel_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (10 << 2)), 0, 0, 0 }, /* atw */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 }, /* singleLevel */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* blockLevel */
};
static asn_SEQUENCE_specifics_t asn_SPC_ATWLevel_specs_1 = {
	sizeof(struct ATWLevel),
	offsetof(struct ATWLevel, _asn_ctx),
	asn_MAP_ATWLevel_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ATWLevel = {
	"ATWLevel",
	"ATWLevel",
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
	asn_DEF_ATWLevel_tags_1,
	sizeof(asn_DEF_ATWLevel_tags_1)
		/sizeof(asn_DEF_ATWLevel_tags_1[0]), /* 1 */
	asn_DEF_ATWLevel_tags_1,	/* Same as above */
	sizeof(asn_DEF_ATWLevel_tags_1)
		/sizeof(asn_DEF_ATWLevel_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ATWLevel_1,
	2,	/* Elements count */
	&asn_SPC_ATWLevel_specs_1	/* Additional specs */
};
