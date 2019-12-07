/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#include "MachAndIas.h"

static asn_TYPE_member_t asn_MBR_MachAndIas_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct MachAndIas, mach),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_SpeedMach,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"mach"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MachAndIas, ias),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_Ias,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"ias"
		},
};
static const ber_tlv_tag_t asn_DEF_MachAndIas_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_MachAndIas_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 1 }, /* mach */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, -1, 0 } /* ias */
};
static asn_SEQUENCE_specifics_t asn_SPC_MachAndIas_specs_1 = {
	sizeof(struct MachAndIas),
	offsetof(struct MachAndIas, _asn_ctx),
	asn_MAP_MachAndIas_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_MachAndIas = {
	"MachAndIas",
	"MachAndIas",
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
	asn_DEF_MachAndIas_tags_1,
	sizeof(asn_DEF_MachAndIas_tags_1)
		/sizeof(asn_DEF_MachAndIas_tags_1[0]), /* 1 */
	asn_DEF_MachAndIas_tags_1,	/* Same as above */
	sizeof(asn_DEF_MachAndIas_tags_1)
		/sizeof(asn_DEF_MachAndIas_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_MachAndIas_1,
	2,	/* Elements count */
	&asn_SPC_MachAndIas_specs_1	/* Additional specs */
};

