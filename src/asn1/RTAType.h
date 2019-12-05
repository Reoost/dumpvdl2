/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#ifndef	_RTAType_H_
#define	_RTAType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum RTAType {
	RTAType_at	= 0,
	RTAType_at_or_before	= 1,
	RTAType_at_or_after	= 2
} e_RTAType;

/* RTAType */
typedef long	 RTAType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RTAType;
asn_struct_free_f RTAType_free;
asn_struct_print_f RTAType_print;
asn_constr_check_f RTAType_constraint;
ber_type_decoder_f RTAType_decode_ber;
der_type_encoder_f RTAType_encode_der;
xer_type_decoder_f RTAType_decode_xer;
xer_type_encoder_f RTAType_encode_xer;
per_type_decoder_f RTAType_decode_uper;
per_type_encoder_f RTAType_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _RTAType_H_ */
#include "asn_internal.h"
