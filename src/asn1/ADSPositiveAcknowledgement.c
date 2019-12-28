/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#include "ADSPositiveAcknowledgement.h"

static asn_TYPE_member_t asn_MBR_ADSPositiveAcknowledgement_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ADSPositiveAcknowledgement, contractNumber),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ContractNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"contractNumber"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ADSPositiveAcknowledgement, requestType),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ADSv2RequestType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"requestType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ADSPositiveAcknowledgement, connectedATSUList),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ConnectedATSUList,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"connectedATSUList"
		},
	{ ATF_POINTER, 1, offsetof(struct ADSPositiveAcknowledgement, emergencyUrgency),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ADSEmergencyUrgencyStatus,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"emergencyUrgency"
		},
};
static const int asn_MAP_ADSPositiveAcknowledgement_oms_1[] = { 3 };
static const ber_tlv_tag_t asn_DEF_ADSPositiveAcknowledgement_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_ADSPositiveAcknowledgement_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* contractNumber */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* requestType */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* connectedATSUList */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* emergencyUrgency */
};
static asn_SEQUENCE_specifics_t asn_SPC_ADSPositiveAcknowledgement_specs_1 = {
	sizeof(struct ADSPositiveAcknowledgement),
	offsetof(struct ADSPositiveAcknowledgement, _asn_ctx),
	asn_MAP_ADSPositiveAcknowledgement_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_ADSPositiveAcknowledgement_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	3,	/* Start extensions */
	5	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ADSPositiveAcknowledgement = {
	"ADSPositiveAcknowledgement",
	"ADSPositiveAcknowledgement",
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
	asn_DEF_ADSPositiveAcknowledgement_tags_1,
	sizeof(asn_DEF_ADSPositiveAcknowledgement_tags_1)
		/sizeof(asn_DEF_ADSPositiveAcknowledgement_tags_1[0]), /* 1 */
	asn_DEF_ADSPositiveAcknowledgement_tags_1,	/* Same as above */
	sizeof(asn_DEF_ADSPositiveAcknowledgement_tags_1)
		/sizeof(asn_DEF_ADSPositiveAcknowledgement_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ADSPositiveAcknowledgement_1,
	4,	/* Elements count */
	&asn_SPC_ADSPositiveAcknowledgement_specs_1	/* Additional specs */
};
