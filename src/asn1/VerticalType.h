/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#ifndef	_VerticalType_H_
#define	_VerticalType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum VerticalType {
	VerticalType_topOfClimb	= 0,
	VerticalType_topOfDescent	= 1,
	VerticalType_startOfClimb	= 2,
	VerticalType_startOfDescent	= 3,
	VerticalType_startOfLevel	= 4,
	VerticalType_startOfSpeedChange	= 5,
	VerticalType_endOfSpeedChange	= 6,
	VerticalType_speedLimit	= 7,
	VerticalType_crossOver	= 8
} e_VerticalType;

/* VerticalType */
typedef BIT_STRING_t	 VerticalType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_VerticalType;
asn_struct_free_f VerticalType_free;
asn_struct_print_f VerticalType_print;
asn_constr_check_f VerticalType_constraint;
ber_type_decoder_f VerticalType_decode_ber;
der_type_encoder_f VerticalType_encode_der;
xer_type_decoder_f VerticalType_decode_xer;
xer_type_encoder_f VerticalType_encode_xer;
per_type_decoder_f VerticalType_decode_uper;
per_type_encoder_f VerticalType_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _VerticalType_H_ */
#include "asn_internal.h"
