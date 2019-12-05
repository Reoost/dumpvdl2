/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "ADSMessageSetVersion2"
 * 	found in "../../../dumpvdl2.asn1/atn-b2_adsc_v2.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#ifndef	_PeriodicContractRequest_H_
#define	_PeriodicContractRequest_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ContractNumber.h"
#include "ReportingRate.h"
#include "Modulus.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct MetInfoModulus;
struct ExtendedProjectedProfileModulus;
struct TOARangeRequestModulus;

/* PeriodicContractRequest */
typedef struct PeriodicContractRequest {
	ContractNumber_t	 contract_number;
	ReportingRate_t	 reporting_rate;
	Modulus_t	*projected_profile_modulus	/* OPTIONAL */;
	Modulus_t	*ground_vector_modulus	/* OPTIONAL */;
	Modulus_t	*air_vector_modulus	/* OPTIONAL */;
	struct MetInfoModulus	*met_info_modulus	/* OPTIONAL */;
	struct ExtendedProjectedProfileModulus	*extended_projected_profile_modulus	/* OPTIONAL */;
	struct TOARangeRequestModulus	*toa_range_modulus	/* OPTIONAL */;
	Modulus_t	*speed_schedule_profile_modulus	/* OPTIONAL */;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	Modulus_t	*rnp_profile_modulus	/* OPTIONAL */;
	Modulus_t	*planned_final_approach_speed_modulus	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PeriodicContractRequest_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PeriodicContractRequest;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "MetInfoModulus.h"
#include "ExtendedProjectedProfileModulus.h"
#include "TOARangeRequestModulus.h"

#endif	/* _PeriodicContractRequest_H_ */
#include "asn_internal.h"
