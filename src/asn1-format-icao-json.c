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

#include <libacars/vstring.h>                   // la_vstring
#include <libacars/dict.h>                      // la_dict
#include <libacars/json.h>
#include "asn1/ACSE-apdu.h"                     // asn_DEF_ACSE_apdu
#include "asn1/ABRT-source.h"                   // ABRT_source_*
#include "asn1/ATCDownlinkMessage.h"            // ATCDownlinkMessage_t and dependencies
#include "asn1/ATCUplinkMessage.h"              // ATCUplinkMessage_t and dependencies
#include "asn1/Associate-result.h"              // Associate_result_*
#include "asn1/CMAircraftMessage.h"             // asn_DEF_AircraftMessage
#include "asn1/CMContactRequest.h"              // asn_DEF_CMContactRequest
#include "asn1/CMGroundMessage.h"               // asn_DEF_CMGroundMessage
#include "asn1/ProtectedAircraftPDUs.h"         // asn_DEF_ProtectedAircraftPDUs
#include "asn1/ProtectedGroundPDUs.h"           // asn_DEF_ProtectedGroundPDUs
#include "asn1/ADSAircraftPDU.h"                // asn_DEF_ADSAircraftPDU
#include "asn1/ADSAircraftPDUs.h"               // asn_DEF_ADSAircraftPDUs
#include "asn1/ADSAccept.h"                     // asn_DEF_ADSAccept
#include "asn1/ADSGroundPDU.h"                  // asn_DEF_ADSGroundPDU
#include "asn1/ADSGroundPDUs.h"                 // asn_DEF_ADSGroundPDUs
#include "asn1/ADSNonCompliance.h"              // asn_DEF_ADSNonCompliance
#include "asn1/ADSPositiveAcknowledgement.h"    // asn_DEF_ADSPositiveAcknowledgement
#include "asn1/ADSRequestContract.h"            // asn_DEF_ADSRequestContract
#include "asn1/ADSReject.h"                     // asn_DEF_ADSReject
#include "asn1/ADSReport.h"                     // asn_DEF_ADSReport
#include "asn1/Release-request-reason.h"        // Release_request_reason_*
#include "asn1/Release-response-reason.h"       // Release_response_reason_*
#include "dumpvdl2.h"                           // XCALLOC, dict_search()
#include <libacars/asn1-util.h>                 // la_asn1_formatter_func, la_asn1_output()
#include <libacars/asn1-format-common.h>                 // common formatters and helper functions
#include "asn1-format-icao.h"                   // *_labels dictionaries

// forward declarations
la_asn1_formatter const asn1_acse_formatter_table_json[];
size_t asn1_acse_formatter_table_json_len;
la_asn1_formatter const asn1_icao_formatter_table_json[];
size_t asn1_icao_formatter_table_json_len;

/************************
 * ASN.1 type formatters
 ************************/

LA_ASN1_FORMATTER_FUNC(asn1_format_icao_as_json) {
	la_asn1_output(p, asn1_icao_formatter_table_json, asn1_icao_formatter_table_json_len, false);
}

LA_ASN1_FORMATTER_FUNC(asn1_format_acse_as_json) {
	la_asn1_output(p, asn1_acse_formatter_table_json, asn1_acse_formatter_table_json_len, false);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SEQUENCE_acse_as_json) {
	la_format_SEQUENCE_as_json(p, asn1_format_acse_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_CHOICE_acse_as_json) {
	la_format_CHOICE_as_json(p, NULL, asn1_format_acse_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Associate_result_as_json) {
	la_format_INTEGER_as_ENUM_as_json(p, Associate_result_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Release_request_reason_as_json) {
	la_format_INTEGER_as_ENUM_as_json(p, Release_request_reason_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Release_response_reason_as_json) {
	la_format_INTEGER_as_ENUM_as_json(p, Release_response_reason_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ABRT_source_as_json) {
	la_format_INTEGER_as_ENUM_as_json(p, ABRT_source_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_CHOICE_icao_as_json) {
	la_format_CHOICE_as_json(p, NULL, asn1_format_icao_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SEQUENCE_icao_as_json) {
	la_format_SEQUENCE_as_json(p, asn1_format_icao_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SEQUENCE_OF_icao_as_json) {
	la_format_SEQUENCE_OF_as_json(p, asn1_format_icao_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ATCDownlinkMsgElementId_as_json) {
	la_format_CHOICE_as_json(p, ATCDownlinkMsgElementId_labels, asn1_format_icao_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ATCUplinkMsgElementId_as_json) {
	la_format_CHOICE_as_json(p, ATCUplinkMsgElementId_labels, asn1_format_icao_as_json);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Code_as_json) {
	Code_t const *code = p.sptr;
	long **cptr = code->list.array;
	la_json_append_int64(p.vstr, p.label,
			*cptr[0] * 1000 +
			*cptr[1] * 100 +
			*cptr[2] * 10 +
			*cptr[3]
			);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DateTime_as_json) {
	DateTime_t const *dtg = p.sptr;
	Date_t const *d = &dtg->date;
	Time_t const *t = &dtg->time;
	la_json_object_start(p.vstr, p.label);
	la_json_append_int64(p.vstr, "year", d->year);
	la_json_append_int64(p.vstr, "month", d->month);
	la_json_append_int64(p.vstr, "day", d->day);
	la_json_append_int64(p.vstr, "hour", t->hours);
	la_json_append_int64(p.vstr, "min", t->minutes);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Timehhmmss_as_json) {
	Timehhmmss_t const *t = p.sptr;
	la_json_object_start(p.vstr, p.label);
	la_json_append_int64(p.vstr, "hour", t->hoursminutes.hours);
	la_json_append_int64(p.vstr, "min", t->hoursminutes.minutes);
	la_json_append_int64(p.vstr, "sec", t->seconds);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Time_as_json) {
	Time_t const *t = p.sptr;
	la_json_object_start(p.vstr, p.label);
	la_json_append_int64(p.vstr, "hour", t->hours);
	la_json_append_int64(p.vstr, "min", t->minutes);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Latitude_as_json) {
	Latitude_t const *lat = p.sptr;
	long const ldir = lat->latitudeDirection;
	char const *ldir_name = la_asn1_value2enum(&asn_DEF_LatitudeDirection, ldir);
	la_json_object_start(p.vstr, p.label);
	switch(lat->latitudeType.present) {
		case LatitudeType_PR_latitudeDegrees:
			la_json_append_int64(p.vstr, "deg", lat->latitudeType.choice.latitudeDegrees);
			break;
		case LatitudeType_PR_latitudeDegreesMinutes:
			la_json_append_int64(p.vstr, "deg", lat->latitudeType.choice.latitudeDegreesMinutes.latitudeWholeDegrees);
			la_json_append_double(p.vstr, "min", lat->latitudeType.choice.latitudeDegreesMinutes.minutesLatLon / 100.0);
			break;
		case LatitudeType_PR_latitudeDMS:
			la_json_append_int64(p.vstr, "deg", lat->latitudeType.choice.latitudeDMS.latitudeWholeDegrees);
			la_json_append_int64(p.vstr, "min", lat->latitudeType.choice.latitudeDMS.latlonWholeMinutes);
			la_json_append_int64(p.vstr, "sec", lat->latitudeType.choice.latitudeDMS.secondsLatLon);
			break;
		case LatitudeType_PR_NOTHING:
			break;
	}
	la_json_append_string(p.vstr, "dir", ldir_name);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Longitude_as_json) {
	Longitude_t const *lon = p.sptr;
	long const ldir = lon->longitudeDirection;
	char const *ldir_name = la_asn1_value2enum(&asn_DEF_LongitudeDirection, ldir);
	la_json_object_start(p.vstr, p.label);
	switch(lon->longitudeType.present) {
		case LongitudeType_PR_longitudeDegrees:
			la_json_append_int64(p.vstr, "deg", lon->longitudeType.choice.longitudeDegrees);
			break;
		case LongitudeType_PR_longitudeDegreesMinutes:
			la_json_append_int64(p.vstr, "deg", lon->longitudeType.choice.longitudeDegreesMinutes.longitudeWholeDegrees);
			la_json_append_double(p.vstr, "min", lon->longitudeType.choice.longitudeDegreesMinutes.minutesLatLon / 100.0);
			break;
		case LongitudeType_PR_longitudeDMS:
			la_json_append_int64(p.vstr, "deg", lon->longitudeType.choice.longitudeDMS.longitudeWholeDegrees);
			la_json_append_int64(p.vstr, "min", lon->longitudeType.choice.longitudeDMS.latLonWholeMinutes);
			la_json_append_int64(p.vstr, "sec", lon->longitudeType.choice.longitudeDMS.secondsLatLon);
			break;
		case LongitudeType_PR_NOTHING:
			break;
	}
	la_json_append_string(p.vstr, "dir", ldir_name);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_AltimeterEnglish_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "inHg", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_AltimeterMetric_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "hPa", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Deg_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "deg", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DepartureMinimumInterval_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DistanceKm_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "km", 0.25);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DistanceNm_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Humidity_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "%%", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DistanceEnglish_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_DistanceMetric_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "km", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Frequencyvhf_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "MHz", 0.005);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Frequencyuhf_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "MHz", 0.025);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Frequencyhf_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kHz", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_LegTime_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_LevelFeet_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "ft", 10);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_LevelFlightLevelMetric_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "m", 10);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Meters_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "m", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_OBJECT_IDENTIFIER_as_json) {
	OBJECT_IDENTIFIER_t const *oid = p.sptr;
	uint64_t fixed_arcs[10];	// Try with fixed space first
	uint64_t *arcs = fixed_arcs;
	int arc_type_size = sizeof(fixed_arcs[0]);
	int arc_slots = sizeof(fixed_arcs)/sizeof(fixed_arcs[0]);

	int count = OBJECT_IDENTIFIER_get_arcs(oid, arcs, arc_type_size, arc_slots);
	// If necessary, reallocate arcs array and try again.
	if(count > arc_slots) {
		arc_slots = count;
		arcs = XCALLOC(arc_slots, arc_type_size);
		count = OBJECT_IDENTIFIER_get_arcs(oid, arcs, arc_type_size, arc_slots);
		ASSERT(count == arc_slots);
	}

	la_json_array_start(p.vstr, p.label);
	for(int i = 0; i < count; i++) {
		la_json_append_int64(p.vstr, NULL, arcs[i]);
	}
	la_json_array_end(p.vstr);
	
	if(arcs != fixed_arcs) {
		XFREE(arcs);
	}
}

// RejectDetails is a CHOICE whose all values are NULLs.  Aliasing them all to
// unique types just to print them with la_asn1_format_label_only_as_json would be an
// unnecessary overengineering.  Handling all values in a single routine is
// simpler, albeit less elegant at first glance.
static LA_ASN1_FORMATTER_FUNC(asn1_format_RejectDetails_as_json) {
	RejectDetails_t const *det = p.sptr;
	switch(det->present) {
		case RejectDetails_PR_aDS_service_unavailable:
			la_json_append_string(p.vstr, p.label, "ADS_service_unavailable");
			break;
		case RejectDetails_PR_undefined_reason:
			la_json_append_string(p.vstr, p.label, "undefined_reason");
			break;
		case RejectDetails_PR_maximum_capacity_exceeded:
			la_json_append_string(p.vstr, p.label, "max_capacity_exceeded");
			break;
		case RejectDetails_PR_reserved:
			la_json_append_string(p.vstr, p.label, "(reserved)");
			break;
		case RejectDetails_PR_waypoint_in_request_not_on_the_route:
			la_json_append_string(p.vstr, p.label, "requested_waypoint_not_on_the_route");
			break;
		case RejectDetails_PR_aDS_contract_not_supported:
			la_json_append_string(p.vstr, p.label, "ADS_contract_not_supported");
			break;
		case RejectDetails_PR_noneOfReportTypesSupported:
			la_json_append_string(p.vstr, p.label, "none_of_report_types_supported");
			break;
		case RejectDetails_PR_noneOfEventTypesSupported:
			la_json_append_string(p.vstr, p.label, "none_of_event_types_supported");
			break;
		case RejectDetails_PR_NOTHING:
		default:
			la_json_append_string(p.vstr, p.label, "none");
	}
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ReportingRate_as_json) {
	ReportingRate_t const *rate = p.sptr;
	switch(rate->present) {
		case ReportingRate_PR_reporting_time_seconds_scale:
			p.sptr = &rate->choice.reporting_time_seconds_scale;
			la_format_INTEGER_with_unit_as_json(p, "sec", 1);
			break;
		case ReportingRate_PR_reporting_time_minutes_scale:
			p.sptr = &rate->choice.reporting_time_minutes_scale;
			la_format_INTEGER_with_unit_as_json(p, "min", 1);
			break;
		default:
			break;
	}
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_RTASecTolerance_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "sec", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_RTATolerance_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Feet_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "ft", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SpeedMetric_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "km/h", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SpeedEnglish_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kts", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SpeedIndicated_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kts", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_SpeedMach_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "", 0.001);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_Temperature_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "C", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_VerticalRateEnglish_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "ft/min", 10);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_VerticalRateMetric_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "m/min", 10);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EstimatedPositionUncertainty_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ADSv2Latitude_as_json) {
	ADSv2Latitude_t const *lat = p.sptr;
	long const ldir = lat->direction;
	char const *ldir_name = la_asn1_value2enum(&asn_DEF_LatitudeDirection, ldir);
	la_json_object_start(p.vstr, p.label);
	la_json_append_int64(p.vstr, "deg", lat->degrees);
	la_json_append_int64(p.vstr, "min", lat->minutes);
	la_json_append_double(p.vstr, "sec", lat->seconds / 10.0);
	la_json_append_string(p.vstr, "dir", ldir_name);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ADSv2Longitude_as_json) {
	ADSv2Longitude_t const *lon = p.sptr;
	long const ldir = lon->direction;
	char const *ldir_name = la_asn1_value2enum(&asn_DEF_LongitudeDirection, ldir);
	la_json_object_start(p.vstr, p.label);
	la_json_append_int64(p.vstr, "deg", lon->degrees);
	la_json_append_int64(p.vstr, "min", lon->minutes);
	la_json_append_double(p.vstr, "sec", lon->seconds / 10.0);
	la_json_append_string(p.vstr, "dir", ldir_name);
	la_json_object_end(p.vstr);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ADSv2Temperature_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "C", 0.25);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ADSv2WindSpeedKts_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kts", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ADSv2WindSpeedKmh_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "km/h", 2);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EmergencyUrgencyStatus_as_json) {
	la_format_BIT_STRING_as_json(p, EmergencyUrgencyStatus_bit_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EPPTimeInterval_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "minutes", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EventTypeNotSupported_as_json) {
	la_format_BIT_STRING_as_json(p, EventTypeNotSupported_bit_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_GrossMass_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kg", 10);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EPPLimitations_as_json) {
	la_format_BIT_STRING_as_json(p, EPPLimitations_bit_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EPPTolETA_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EPPTolGCDistance_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_EPUChangeTolerance_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_GroundSpeed_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "kts", 0.5);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_GroundTrack_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "deg", 0.05);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_LateralDeviationThreshold_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_MachNumberTolerance_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_ReportTypeNotSupported_as_json) {
	la_format_BIT_STRING_as_json(p, ReportTypeNotSupported_bit_labels);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_RNPValue_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_TurbulenceEDRValue_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "m^2/s^3", 0.01);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_TurbulenceMinutesInThePast_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 0.5);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_TurbulenceObservationWindow_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "min", 1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_TurnRadius_as_json) {
	la_format_INTEGER_with_unit_as_json(p, "nm", 0.1);
}

static LA_ASN1_FORMATTER_FUNC(asn1_format_VerticalType_as_json) {
	la_format_BIT_STRING_as_json(p, VerticalType_bit_labels);
}

la_asn1_formatter const asn1_icao_formatter_table_json[] = {
	// atn-b1_cpdlc-v1.asn1
	{ .type = &asn_DEF_AircraftAddress, .format = la_asn1_format_any_as_string_as_json, .label = "aircraft_address" },
	{ .type = &asn_DEF_AirInitiatedApplications, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "air_initiated_apps" },
	{ .type = &asn_DEF_AirOnlyInitiatedApplications, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "air_only_initiated_apps" },
	{ .type = &asn_DEF_Airport, .format = la_asn1_format_any_as_string_as_json, .label = "airport" },
	{ .type = &asn_DEF_AirportDeparture, .format = la_asn1_format_any_as_string_as_json, .label = "departure_airport" },
	{ .type = &asn_DEF_AirportDestination, .format = la_asn1_format_any_as_string_as_json, .label = "destination_airport" },
	{ .type = &asn_DEF_Altimeter, .format = asn1_format_CHOICE_icao_as_json, .label = "altimeter" },
	{ .type = &asn_DEF_AltimeterEnglish, .format = asn1_format_AltimeterEnglish_as_json, .label = "altimeter_english" },
	{ .type = &asn_DEF_AltimeterMetric, .format = asn1_format_AltimeterMetric_as_json, .label = "altimeter_metric" },
	{ .type = &asn_DEF_ATCDownlinkMessage, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atc_downlink_message" },
	{ .type = &asn_DEF_ATCDownlinkMessageData, .format = asn1_format_SEQUENCE_icao_as_json, .label = "msg_data" },
	{ .type = &asn_DEF_ATCDownlinkMsgElementId, .format = asn1_format_ATCDownlinkMsgElementId_as_json, .label = "msg_element" },
	{ .type = &asn_DEF_ATCDownlinkMsgElementIdSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "msg_elements" },
	{ .type = &asn_DEF_ATCMessageHeader, .format = asn1_format_SEQUENCE_icao_as_json, .label = "header" },
	{ .type = &asn_DEF_ATCUplinkMessage, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atc_uplink_message" },
	{ .type = &asn_DEF_ATCUplinkMessageData, .format = asn1_format_SEQUENCE_icao_as_json, .label = "msg_data" },
	{ .type = &asn_DEF_ATCUplinkMsgElementId, .format = asn1_format_ATCUplinkMsgElementId_as_json, .label = "msg_element" },
	{ .type = &asn_DEF_ATCUplinkMsgElementIdSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "msg_elements" },
	{ .type = &asn_DEF_ATISCode, .format = la_asn1_format_any_as_string_as_json, .label = "atis_code" },
	{ .type = &asn_DEF_ATSRouteDesignator, .format = la_asn1_format_any_as_string_as_json, .label = "ats_route" },
	{ .type = &asn_DEF_ATWAlongTrackWaypoint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atw_along_track_waypoint" },
	{ .type = &asn_DEF_ATWAlongTrackWaypointSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "along_track_waypoints" },
	{ .type = &asn_DEF_ATWDistance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atw_distance" },
	{ .type = &asn_DEF_ATWDistanceTolerance, .format = la_asn1_format_ENUM_as_json, .label = "atw_distance_tolerance" },
	{ .type = &asn_DEF_ATWLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atw_level" },
	{ .type = &asn_DEF_ATWLevelSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "atw_levels" },
	{ .type = &asn_DEF_ATWLevelTolerance, .format = la_asn1_format_ENUM_as_json, .label = "atw_level_tolerance" },
	{ .type = &asn_DEF_BlockLevel, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "block_level" },
	{ .type = &asn_DEF_ClearanceType, .format = la_asn1_format_ENUM_as_json, .label = "clearance_type" },
	{ .type = &asn_DEF_Code, .format = asn1_format_Code_as_json, .label = "code" },
	{ .type = &asn_DEF_ControlledTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "controlled_time" },
	{ .type = &asn_DEF_DateTimeDepartureETD, .format = asn1_format_DateTime_as_json, .label = "departure_time" },
	{ .type = &asn_DEF_DateTimeGroup, .format = asn1_format_SEQUENCE_icao_as_json, .label = "timestamp" },
	{ .type = &asn_DEF_DegreeIncrement, .format = asn1_format_Deg_as_json, .label = "degree_increment" },
	{ .type = &asn_DEF_Degrees, .format = asn1_format_CHOICE_icao_as_json, .label = "degrees" },
	{ .type = &asn_DEF_DegreesMagnetic, .format = asn1_format_Deg_as_json, .label = "degrees_magnetic"},
	{ .type = &asn_DEF_DegreesTrue, .format = asn1_format_Deg_as_json, .label = "degrees_true" },
	{ .type = &asn_DEF_DepartureClearance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "departure_clearance" },
	{ .type = &asn_DEF_DepartureMinimumInterval, .format = asn1_format_DepartureMinimumInterval_as_json, .label = "minimum_interval_of_departures" },
	{ .type = &asn_DEF_Direction, .format = la_asn1_format_ENUM_as_json, .label = "direction" },
	{ .type = &asn_DEF_DirectionDegrees, .format = asn1_format_SEQUENCE_icao_as_json, .label = "direction_degrees" },
	{ .type = &asn_DEF_Distance, .format = asn1_format_CHOICE_icao_as_json, .label = "distance" },
	{ .type = &asn_DEF_DistanceKm, .format = asn1_format_DistanceKm_as_json, .label = "distance" },
	{ .type = &asn_DEF_DistanceNm, .format = asn1_format_DistanceNm_as_json, .label = "distance" },
	{ .type = &asn_DEF_DistanceSpecified, .format = asn1_format_CHOICE_icao_as_json, .label = "distance_specified" },
	{ .type = &asn_DEF_DistanceSpecifiedDirection, .format = asn1_format_SEQUENCE_icao_as_json, .label = "distance_specified_direction" },
	{ .type = &asn_DEF_DistanceSpecifiedDirectionTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "distance_specified_direction_time" },
	{ .type = &asn_DEF_DistanceSpecifiedKm, .format = asn1_format_DistanceMetric_as_json, .label = "offset" },
	{ .type = &asn_DEF_DistanceSpecifiedNm, .format = asn1_format_DistanceEnglish_as_json, .label = "offset" },
	{ .type = &asn_DEF_DMVersionNumber, .format = la_asn1_format_long_as_json, .label = "version" },
	{ .type = &asn_DEF_ErrorInformation, .format = la_asn1_format_ENUM_as_json, .label = "error_info" },
	{ .type = &asn_DEF_Facility, .format = asn1_format_CHOICE_icao_as_json, .label = "facility" },
	{ .type = &asn_DEF_FacilityDesignation, .format = la_asn1_format_any_as_string_as_json, .label = "facility_designation" },
	{ .type = &asn_DEF_FacilityDesignationAltimeter, .format = asn1_format_SEQUENCE_icao_as_json, .label = "facility_designation_altimeter" },
	{ .type = &asn_DEF_FacilityDesignationATISCode, .format = asn1_format_SEQUENCE_icao_as_json, .label = "facility_designation_atis_code" },
	{ .type = &asn_DEF_FacilityFunction, .format = la_asn1_format_ENUM_as_json, .label = "facility_function" },
	{ .type = &asn_DEF_FacilityName, .format = la_asn1_format_any_as_string_as_json, .label = "facility_name" },
	{ .type = &asn_DEF_Fix, .format = la_asn1_format_any_as_string_as_json, .label = "fix" },
	{ .type = &asn_DEF_FixName, .format = asn1_format_SEQUENCE_icao_as_json, .label = "fix_name" },
	{ .type = &asn_DEF_FixNext, .format = asn1_format_CHOICE_icao_as_json, .label = "fix_next" },
	{ .type = &asn_DEF_FixNextPlusOne, .format = asn1_format_CHOICE_icao_as_json, .label = "fix_next_plus_one" },
	{ .type = &asn_DEF_FlightInformation, .format = asn1_format_CHOICE_icao_as_json, .label = "flight_info" },
	{ .type = &asn_DEF_FreeText, .format = la_asn1_format_any_as_string_as_json, .label = "free_text" },
	{ .type = &asn_DEF_Frequency, .format = asn1_format_CHOICE_icao_as_json, .label = "frequency" },
	{ .type = &asn_DEF_Frequencyhf, .format = asn1_format_Frequencyhf_as_json, .label = "hf" },
	{ .type = &asn_DEF_Frequencysatchannel, .format = la_asn1_format_any_as_string_as_json, .label = "satcom_channel" },
	{ .type = &asn_DEF_Frequencyuhf, .format = asn1_format_Frequencyuhf_as_json, .label = "uhf" },
	{ .type = &asn_DEF_Frequencyvhf, .format = asn1_format_Frequencyvhf_as_json, .label = "vhf" },
	{ .type = &asn_DEF_FurtherInstructions, .format = asn1_format_SEQUENCE_icao_as_json, .label = "further_instructions" },
	{ .type = &asn_DEF_GroundInitiatedApplications, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "ground_initiated_apps" },
	{ .type = &asn_DEF_GroundOnlyInitiatedApplications, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "ground_only_initiated_apps" },
	{ .type = &asn_DEF_Holdatwaypoint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "hold_at_waypoint" },
	{ .type = &asn_DEF_HoldatwaypointSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "holding_points" },
	{ .type = &asn_DEF_HoldatwaypointSpeedHigh, .format = asn1_format_CHOICE_icao_as_json, .label = "holding_speed_high" },
	{ .type = &asn_DEF_HoldatwaypointSpeedLow, .format = asn1_format_CHOICE_icao_as_json, .label = "holding_speed_low" },
	{ .type = &asn_DEF_HoldClearance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "hold_clearance" },
	{ .type = &asn_DEF_Humidity, .format = asn1_format_Humidity_as_json, .label = "humidity" },
	{ .type = &asn_DEF_Icing, .format = la_asn1_format_ENUM_as_json, .label = "icing" },
	{ .type = &asn_DEF_InterceptCourseFrom, .format = asn1_format_SEQUENCE_icao_as_json, .label = "intercept_course_from" },
	{ .type = &asn_DEF_InterceptCourseFromSelection, .format = asn1_format_CHOICE_icao_as_json, .label = "intercept_course_from_selection" },
	{ .type = &asn_DEF_InterceptCourseFromSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "intercept_courses" },
	{ .type = &asn_DEF_Latitude, .format = asn1_format_Latitude_as_json, .label = "lat" },
	{ .type = &asn_DEF_LatitudeDirection, .format = la_asn1_format_ENUM_as_json, .label = "direction" },
	{ .type = &asn_DEF_LatitudeLongitude, .format = asn1_format_SEQUENCE_icao_as_json, .label = "lat_lon" },
	{ .type = &asn_DEF_LatitudeReportingPoints, .format = asn1_format_SEQUENCE_icao_as_json, .label = "lat_rep_points" },
	{ .type = &asn_DEF_LatitudeType, .format = asn1_format_CHOICE_icao_as_json, .label = "lat_type" },
	{ .type = &asn_DEF_LatLonReportingPoints, .format = asn1_format_CHOICE_icao_as_json, .label = "lat_lon_rep_points" },
	{ .type = &asn_DEF_LegDistance, .format = asn1_format_CHOICE_icao_as_json, .label = "leg_distance" },
	{ .type = &asn_DEF_LegDistanceEnglish, .format = asn1_format_DistanceEnglish_as_json, .label = "leg_distance" },
	{ .type = &asn_DEF_LegDistanceMetric, .format = asn1_format_DistanceMetric_as_json, .label = "leg_distance" },
	{ .type = &asn_DEF_LegTime, .format = asn1_format_LegTime_as_json, .label = "leg_time" },
	{ .type = &asn_DEF_LegType, .format = asn1_format_CHOICE_icao_as_json, .label = "leg_type" },
	{ .type = &asn_DEF_Level, .format = asn1_format_CHOICE_icao_as_json, .label = "level" },
	{ .type = &asn_DEF_LevelFeet, .format = asn1_format_LevelFeet_as_json, .label = "flight_level" },
	{ .type = &asn_DEF_LevelFlightLevel, .format = la_asn1_format_long_as_json, .label = "flight_level" },
	{ .type = &asn_DEF_LevelFlightLevelMetric, .format = asn1_format_LevelFlightLevelMetric_as_json, .label = "flight_level" },
	{ .type = &asn_DEF_LevelLevel, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "level_level" },
	{ .type = &asn_DEF_LevelMeters, .format = asn1_format_Meters_as_json, .label = "flight_level" },
	{ .type = &asn_DEF_LevelPosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_position" },
	{ .type = &asn_DEF_LevelProcedureName, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_procedure_name" },
	{ .type = &asn_DEF_LevelsOfFlight, .format = asn1_format_CHOICE_icao_as_json, .label = "levels_of_flights" },
	{ .type = &asn_DEF_LevelSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_speed" },
	{ .type = &asn_DEF_LevelSpeedSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_speed_speed" },
	{ .type = &asn_DEF_LevelTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_time" },
	{ .type = &asn_DEF_LevelType, .format = asn1_format_CHOICE_icao_as_json, .label = "level_type" },
	{ .type = &asn_DEF_LogicalAck, .format = la_asn1_format_ENUM_as_json, .label = "logical_ack" },
	{ .type = &asn_DEF_Longitude, .format = asn1_format_Longitude_as_json, .label = "lon" },
	{ .type = &asn_DEF_LongitudeDirection, .format = la_asn1_format_ENUM_as_json, .label = "direction" },
	{ .type = &asn_DEF_LongitudeReportingPoints, .format = asn1_format_SEQUENCE_icao_as_json, .label = "lon_rep_points" },
	{ .type = &asn_DEF_LongitudeType, .format = asn1_format_CHOICE_icao_as_json, .label = "lon_type" },
	{ .type = &asn_DEF_MsgIdentificationNumber, .format = la_asn1_format_long_as_json, .label = "msg_id" },
	{ .type = &asn_DEF_MsgReferenceNumber, .format = la_asn1_format_long_as_json, .label = "msg_ref" },
	{ .type = &asn_DEF_Navaid, .format = asn1_format_SEQUENCE_icao_as_json, .label = "navaid" },
	{ .type = &asn_DEF_NavaidName, .format = la_asn1_format_any_as_string_as_json, .label = "navaid_name" },
	{ .type = &asn_DEF_PersonsOnBoard, .format = la_asn1_format_long_as_json, .label = "persons_on_board" },
	{ .type = &asn_DEF_PlaceBearing, .format = asn1_format_SEQUENCE_icao_as_json, .label = "place_bearing" },
	{ .type = &asn_DEF_PlaceBearingDistance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "place_bearing_distance" },
	{ .type = &asn_DEF_PlaceBearingPlaceBearing, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "place_bearing_place_bearing" },
	{ .type = &asn_DEF_PMCPDLCProviderAbortReason, .format = la_asn1_format_ENUM_as_json, .label = "cpdlc_provider_abort_reason" },
	{ .type = &asn_DEF_PMCPDLCUserAbortReason, .format = la_asn1_format_ENUM_as_json, .label = "cpdlc_user_abort_reason" },
	{ .type = &asn_DEF_Position, .format = asn1_format_CHOICE_icao_as_json, .label = "position" },
	{ .type = &asn_DEF_PositionDegrees, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_degrees" },
	{ .type = &asn_DEF_PositionDistanceSpecifiedDirection, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_dist_specified_direction" },
	{ .type = &asn_DEF_PositionLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_level" },
	{ .type = &asn_DEF_PositionLevelLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_level_level" },
	{ .type = &asn_DEF_PositionLevelSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_level_speed" },
	{ .type = &asn_DEF_PositionPosition, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "position_position" },
	{ .type = &asn_DEF_PositionProcedureName, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_procedure_name" },
	{ .type = &asn_DEF_PositionReport, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_report" },
	{ .type = &asn_DEF_PositionRouteClearanceIndex, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_rte_clearance_idx" },
	{ .type = &asn_DEF_PositionSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_speed" },
	{ .type = &asn_DEF_PositionSpeedSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_speed_speed" },
	{ .type = &asn_DEF_PositionTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_time" },
	{ .type = &asn_DEF_PositionTimeLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_time_level" },
	{ .type = &asn_DEF_PositionTimeTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_time_time" },
	{ .type = &asn_DEF_PositionUnitNameFrequency, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position_unit_name_frequency" },
	{ .type = &asn_DEF_PosReportHeading, .format = asn1_format_CHOICE_icao_as_json, .label = "heading" },
	{ .type = &asn_DEF_PosReportTrackAngle, .format = asn1_format_CHOICE_icao_as_json, .label = "trk_angle" },
	{ .type = &asn_DEF_Procedure, .format = la_asn1_format_any_as_string_as_json, .label = "procedure" },
	{ .type = &asn_DEF_ProcedureApproach, .format = asn1_format_SEQUENCE_icao_as_json, .label = "approach_procedure" },
	{ .type = &asn_DEF_ProcedureArrival, .format = asn1_format_SEQUENCE_icao_as_json, .label = "arrival_procedure" },
	{ .type = &asn_DEF_ProcedureDeparture, .format = asn1_format_SEQUENCE_icao_as_json, .label = "departure_procedure" },
	{ .type = &asn_DEF_ProcedureName, .format = asn1_format_SEQUENCE_icao_as_json, .label = "procedure_name" },
	{ .type = &asn_DEF_ProcedureTransition, .format = la_asn1_format_any_as_string_as_json, .label = "procedure_transition" },
	{ .type = &asn_DEF_ProcedureType, .format = la_asn1_format_ENUM_as_json, .label = "procedure_type" },
	{ .type = &asn_DEF_ProtectedAircraftPDUs, .format = asn1_format_CHOICE_icao_as_json, .label = "protected_aircraft_pdus" },
	{ .type = &asn_DEF_ProtectedGroundPDUs, .format = asn1_format_CHOICE_icao_as_json, .label = "protected_ground_pdus" },
	{ .type = &asn_DEF_PublishedIdentifier, .format = asn1_format_CHOICE_icao_as_json, .label = "published_identifier" },
	{ .type = &asn_DEF_RemainingFuel, .format = asn1_format_Time_as_json, .label = "remaining_fuel" },
	{ .type = &asn_DEF_RemainingFuelPersonsOnBoard, .format = asn1_format_SEQUENCE_icao_as_json, .label = "remaining_fuel_persons_onboard" },
	{ .type = &asn_DEF_ReportedWaypointLevel, .format = asn1_format_CHOICE_icao_as_json, .label = "reported_waypoint_level" },
	{ .type = &asn_DEF_ReportedWaypointPosition, .format = asn1_format_CHOICE_icao_as_json, .label = "reported_waypoint_position" },
	{ .type = &asn_DEF_ReportedWaypointTime, .format = asn1_format_Time_as_json, .label = "reported_waypoint_time" },
	{ .type = &asn_DEF_ReportingPoints, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rep_points" },
	{ .type = &asn_DEF_RevisionNumber, .format = la_asn1_format_long_as_json, .label = "revision_number" },
	{ .type = &asn_DEF_RouteAndLevels, .format = asn1_format_SEQUENCE_icao_as_json, .label = "route_levels" },
	{ .type = &asn_DEF_RouteClearance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "route_clearance" },
	{ .type = &asn_DEF_RouteClearanceConstrainedData, .format = asn1_format_SEQUENCE_icao_as_json, .label = "route_clearance_data" },
	{ .type = &asn_DEF_RouteClearanceIndex, .format = la_asn1_format_long_as_json, .label = "route_clearance_idx" },
	{ .type = &asn_DEF_RouteClearanceSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "route_clearance_seq" },
	{ .type = &asn_DEF_RouteInformation, .format = asn1_format_CHOICE_icao_as_json, .label = "route_info" },
	{ .type = &asn_DEF_RouteInformationAdditional, .format = asn1_format_SEQUENCE_icao_as_json, .label = "additional_route_info" },
	{ .type = &asn_DEF_RouteInformationSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "route" },
	{ .type = &asn_DEF_RTARequiredTimeArrival, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rta_required_time_arrival" },
	{ .type = &asn_DEF_RTARequiredTimeArrivalSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "required_arrivalival_times" },
	{ .type = &asn_DEF_RTATime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rta_time" },
	{ .type = &asn_DEF_RTATolerance, .format = asn1_format_RTATolerance_as_json, .label = "rta_tolerance" },
	{ .type = &asn_DEF_Runway, .format = asn1_format_SEQUENCE_icao_as_json, .label = "runway" },
	{ .type = &asn_DEF_RunwayArrival, .format = asn1_format_SEQUENCE_icao_as_json, .label = "arrival_runway" },
	{ .type = &asn_DEF_RunwayConfiguration, .format = la_asn1_format_ENUM_as_json, .label = "runway_configuration" },
	{ .type = &asn_DEF_RunwayDeparture, .format = asn1_format_SEQUENCE_icao_as_json, .label = "departure_runway" },
	{ .type = &asn_DEF_RunwayDirection, .format = la_asn1_format_long_as_json, .label = "runway_direction" },
	{ .type = &asn_DEF_RunwayRVR, .format = asn1_format_SEQUENCE_icao_as_json, .label = "runway_rvr" },
	{ .type = &asn_DEF_RVR, .format = asn1_format_CHOICE_icao_as_json, .label = "rvr" },
	{ .type = &asn_DEF_RVRFeet, .format = asn1_format_Feet_as_json, .label = "rvr" },
	{ .type = &asn_DEF_RVRMeters, .format = asn1_format_Meters_as_json, .label = "rvr" },
	{ .type = &asn_DEF_Speed, .format = asn1_format_CHOICE_icao_as_json, .label = "speed" },
	{ .type = &asn_DEF_SpeedGround, .format = asn1_format_SpeedEnglish_as_json, .label = "ground_speed" },
	{ .type = &asn_DEF_SpeedGroundMetric, .format = asn1_format_SpeedMetric_as_json, .label = "ground_speed" },
	{ .type = &asn_DEF_SpeedIndicated, .format = asn1_format_SpeedIndicated_as_json, .label = "indicated_airspeed" },
	{ .type = &asn_DEF_SpeedIndicatedMetric, .format = asn1_format_SpeedMetric_as_json, .label = "indicated_airspeed" },
	{ .type = &asn_DEF_SpeedMach, .format = asn1_format_SpeedMach_as_json, .label = "mach" },
	{ .type = &asn_DEF_SpeedSpeed, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "speed_speed" },
	{ .type = &asn_DEF_SpeedTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "speed_time" },
	{ .type = &asn_DEF_SpeedTrue, .format = asn1_format_SpeedEnglish_as_json, .label = "true_airspeed" },
	{ .type = &asn_DEF_SpeedTrueMetric, .format = asn1_format_SpeedMetric_as_json, .label = "true_airspeed" },
	{ .type = &asn_DEF_SpeedType, .format = la_asn1_format_ENUM_as_json, .label = "speed_type" },
	{ .type = &asn_DEF_SpeedTypeSpeedTypeSpeedType, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "speed_type_speed_type_speed_type" },
	{ .type = &asn_DEF_SpeedTypeSpeedTypeSpeedTypeSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "speed_type_speed_type_speed_type_speed" },
	{ .type = &asn_DEF_Temperature, .format = asn1_format_Temperature_as_json, .label = "temperature" },
	{ .type = &asn_DEF_Time, .format = asn1_format_Time_as_json, .label = "time" },
	{ .type = &asn_DEF_TimeDeparture, .format = asn1_format_SEQUENCE_icao_as_json, .label = "departure_time" },
	{ .type = &asn_DEF_TimeDepAllocated, .format = asn1_format_Time_as_json, .label = "time_departure_allocated" },
	{ .type = &asn_DEF_TimeDepClearanceExpected, .format = asn1_format_Time_as_json, .label = "time_departure_clearance_expected" },
	{ .type = &asn_DEF_TimeDistanceSpecifiedDirection, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_distance_specified_direction" },
	{ .type = &asn_DEF_TimeDistanceToFromPosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_distance_to_from_position" },
	{ .type = &asn_DEF_TimeETAatFixNext, .format = asn1_format_Time_as_json, .label = "eta_at_fix_next" },
	{ .type = &asn_DEF_TimeETAatDest, .format = asn1_format_Time_as_json, .label = "eta_at_dest" },
	{ .type = &asn_DEF_TimeLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_level" },
	{ .type = &asn_DEF_TimePosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_position" },
	{ .type = &asn_DEF_TimePositionLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_position_level" },
	{ .type = &asn_DEF_TimePositionLevelSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_position_level_speed" },
	{ .type = &asn_DEF_TimeSeconds, .format = la_asn1_format_long_as_json, .label = "sec" },
	{ .type = &asn_DEF_TimeSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_speed" },
	{ .type = &asn_DEF_TimeSpeedSpeed, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_speed_speed" },
	{ .type = &asn_DEF_TimeTime, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "time_time" },
	{ .type = &asn_DEF_TimeToFromPosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_to_from_position" },
	{ .type = &asn_DEF_TimeTolerance, .format = la_asn1_format_ENUM_as_json, .label = "time_tolerance" },
	{ .type = &asn_DEF_TimeUnitNameFrequency, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time_unit_name_frequency" },
	{ .type = &asn_DEF_Timehhmmss, .format = asn1_format_Timehhmmss_as_json, .label = "time" },
	{ .type = &asn_DEF_ToFrom, .format = la_asn1_format_ENUM_as_json, .label = "to_from" },
	{ .type = &asn_DEF_ToFromPosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "to_from_position" },
	{ .type = &asn_DEF_TrafficType, .format = la_asn1_format_ENUM_as_json, .label = "traffic_type" },
	{ .type = &asn_DEF_Turbulence, .format = la_asn1_format_ENUM_as_json, .label = "turbulence" },
	{ .type = &asn_DEF_UnitName, .format = asn1_format_SEQUENCE_icao_as_json, .label = "unit_name" },
	{ .type = &asn_DEF_UnitNameFrequency, .format = asn1_format_SEQUENCE_icao_as_json, .label = "unit_name_frequency" },
	{ .type = &asn_DEF_VerticalChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "vertical_change" },
	{ .type = &asn_DEF_VerticalDirection, .format = la_asn1_format_ENUM_as_json, .label = "vertical_direction" },
	{ .type = &asn_DEF_VerticalRate, .format = asn1_format_CHOICE_icao_as_json, .label = "vertical_rate" },
	{ .type = &asn_DEF_VerticalRateEnglish, .format = asn1_format_VerticalRateEnglish_as_json, .label = "vertical_rate" },
	{ .type = &asn_DEF_VerticalRateMetric, .format = asn1_format_VerticalRateMetric_as_json, .label = "vertical_rate" },
	{ .type = &asn_DEF_WaypointSpeedLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "waypoint_speed_level" },
	{ .type = &asn_DEF_WaypointSpeedLevelSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "waypoints_speeds_and_levels" },
	{ .type = &asn_DEF_WindDirection, .format = asn1_format_Deg_as_json, .label = "wind_direction" },
	{ .type = &asn_DEF_Winds, .format = asn1_format_SEQUENCE_icao_as_json, .label = "winds" },
	{ .type = &asn_DEF_WindSpeed, .format = asn1_format_CHOICE_icao_as_json, .label = "wind_speed" },
	{ .type = &asn_DEF_WindSpeedEnglish, .format = asn1_format_SpeedEnglish_as_json, .label = "wind_speed" },
	{ .type = &asn_DEF_WindSpeedMetric, .format = asn1_format_SpeedMetric_as_json, .label = "wind_speed" },
	// atn-b1_cm.asn1
	{ .type = &asn_DEF_APAddress, .format = asn1_format_CHOICE_icao_as_json, .label = "ap_address" },
	{ .type = &asn_DEF_AEQualifier, .format = la_asn1_format_long_as_json, .label = "ae_qualifier" },
	{ .type = &asn_DEF_AEQualifierVersion, .format = asn1_format_SEQUENCE_icao_as_json, .label = "ae_qualifier_version" },
	{ .type = &asn_DEF_AEQualifierVersionAddress, .format = asn1_format_SEQUENCE_icao_as_json, .label = "ae_qualifier_version_address" },
	{ .type = &asn_DEF_ARS, .format = la_asn1_format_OCTET_STRING_as_json, .label = "ars" },
	{ .type = &asn_DEF_AircraftFlightIdentification, .format = la_asn1_format_any_as_string_as_json, .label = "flight_id" },
	{ .type = &asn_DEF_CMAbortReason, .format = la_asn1_format_ENUM_as_json, .label = "atn_context_mgmt_abort_reason" },
	{ .type = &asn_DEF_CMAircraftMessage, .format = asn1_format_CHOICE_icao_as_json, .label = "cm_aircraft_message" },
	{ .type = &asn_DEF_CMGroundMessage, .format = asn1_format_CHOICE_icao_as_json, .label = "cm_ground_message" },
	{ .type = &asn_DEF_CMContactRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atn_context_mgmt_contact_request" },
	{ .type = &asn_DEF_CMContactResponse, .format = la_asn1_format_ENUM_as_json, .label = "atn_context_mgmt_contact_response" },
	{ .type = &asn_DEF_CMForwardRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atn_context_mgmt_forward_request" },
	{ .type = &asn_DEF_CMForwardResponse, .format = la_asn1_format_ENUM_as_json, .label = "atn_context_mgmt_forward_response" },
	{ .type = &asn_DEF_CMLogonRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atn_context_mgmt_logon_request" },
	{ .type = &asn_DEF_CMLogonResponse, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atn_context_mgmt_logon_response" },
	{ .type = &asn_DEF_CMUpdate, .format = asn1_format_SEQUENCE_icao_as_json, .label = "atn_context_mgmt_update" },
	{ .type = &asn_DEF_Date, .format = asn1_format_SEQUENCE_icao_as_json, .label = "date" },
	{ .type = &asn_DEF_Day, .format = la_asn1_format_long_as_json, .label = "day" },
	{ .type = &asn_DEF_LocSysNselTsel, .format = la_asn1_format_OCTET_STRING_as_json, .label = "loc_sys_nsel_tsel" },
	{ .type = &asn_DEF_LongTsap, .format = asn1_format_SEQUENCE_icao_as_json, .label = "long_tsap" },
	{ .type = &asn_DEF_Month, .format = la_asn1_format_long_as_json, .label = "month" },
	{ .type = &asn_DEF_OCTET_STRING, .format = la_asn1_format_OCTET_STRING_as_json, .label = "octet_string" }, // ?
	{ .type = &asn_DEF_RDP, .format = la_asn1_format_OCTET_STRING_as_json, .label = "rdp" },
	{ .type = &asn_DEF_ShortTsap, .format = asn1_format_SEQUENCE_icao_as_json, .label = "short_tsap" },
	{ .type = &asn_DEF_Timehours, .format = la_asn1_format_long_as_json, .label = "hour" },
	{ .type = &asn_DEF_Timeminutes, .format = la_asn1_format_long_as_json, .label = "min" },
	{ .type = &asn_DEF_VersionNumber, .format = la_asn1_format_long_as_json, .label = "version" },
	{ .type = &asn_DEF_Year, .format = la_asn1_format_long_as_json, .label = "year" },
	// atn-b1_pmadsc.asn1
	{ .type = &asn_DEF_ADSAircraftPDU, .format = asn1_format_CHOICE_icao_as_json, .label = "adsc_aircraft_pdu" },
	{ .type = &asn_DEF_ADSAircraftPDUs, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_aircraft_pdus" },
	{ .type = &asn_DEF_ADSGroundPDU, .format = asn1_format_CHOICE_icao_as_json, .label = "adsc_ground_pdu" },
	{ .type = &asn_DEF_ADSGroundPDUs, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_ground_pdus" },
	{ .type = &asn_DEF_CancelAllContracts, .format = la_asn1_format_label_only_as_json, .label = "adsc_cancel_all_contracts" },
	{ .type = &asn_DEF_CancelContract, .format = asn1_format_CHOICE_icao_as_json, .label = "adsc_cancel_contract" },
	{ .type = &asn_DEF_CancelPositiveAcknowledgement, .format = la_asn1_format_ENUM_as_json, .label = "adsc_cancel_ack" },
	{ .type = &asn_DEF_CancelRejectReason, .format = asn1_format_SEQUENCE_icao_as_json, .label = "cancel_reject_reason" },
	{ .type = &asn_DEF_ProviderAbortReason, .format = la_asn1_format_ENUM_as_json, .label = "provider_abort_reason" },
	{ .type = &asn_DEF_PMADSCDateTimeGroup, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_msg_timestamp" },
	{ .type = &asn_DEF_PMADSCTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time" },
	{ .type = &asn_DEF_RejectReason, .format = la_asn1_format_ENUM_as_json, .label = "reject_reason" },
	{ .type = &asn_DEF_RequestType, .format = la_asn1_format_ENUM_as_json, .label = "request_type" },
	{ .type = &asn_DEF_UserAbortReason, .format = la_asn1_format_ENUM_as_json, .label = "adsc_user_abort" },
	// atn-b2_adsc_v2.asn1
	{ .type = &asn_DEF_AAISAvailability, .format = la_asn1_format_bool_as_json, .label = "aais_available" },
	{ .type = &asn_DEF_ADSAccept, .format = asn1_format_CHOICE_icao_as_json, .label = "adsc_contract_request_accept" },
	{ .type = &asn_DEF_ADSDataReport, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_data" },
	{ .type = &asn_DEF_ADSEmergencyUrgencyStatus, .format = asn1_format_EmergencyUrgencyStatus_as_json, .label = "emergency_urgency_status" },
	{ .type = &asn_DEF_ADSNonCompliance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_non_compliance_notification" },
	{ .type = &asn_DEF_ADSPositiveAcknowledgement, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_ack" },
	{ .type = &asn_DEF_ADSReject, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_reject" },
	{ .type = &asn_DEF_ADSReport, .format = asn1_format_CHOICE_icao_as_json, .label = "adsc_report" },
	{ .type = &asn_DEF_ADSRequestContract, .format = asn1_format_CHOICE_icao_as_json, .label = "request_contract" },
	{ .type = &asn_DEF_ADSv2DateTimeGroup, .format = asn1_format_SEQUENCE_icao_as_json, .label = "timestamp" },
	{ .type = &asn_DEF_ADSv2Latitude, .format = asn1_format_ADSv2Latitude_as_json, .label = "lat" },
	{ .type = &asn_DEF_ADSv2LatitudeLongitude, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_lat_lon" },
	{ .type = &asn_DEF_ADSv2Level, .format = asn1_format_LevelFeet_as_json, .label = "alt" },
	{ .type = &asn_DEF_ADSv2Longitude, .format = asn1_format_ADSv2Longitude_as_json, .label = "lon" },
	{ .type = &asn_DEF_ADSv2RequestType, .format = la_asn1_format_ENUM_as_json, .label = "request_type" },
	{ .type = &asn_DEF_ADSv2Temperature, .format = asn1_format_ADSv2Temperature_as_json, .label = "temperature" },
	{ .type = &asn_DEF_ADSv2Turbulence, .format = asn1_format_SEQUENCE_icao_as_json, .label = "turbulence" },
	{ .type = &asn_DEF_ADSv2VerticalRate, .format = asn1_format_VerticalRateEnglish_as_json, .label = "vertical_rate" },
	{ .type = &asn_DEF_ADSv2WindSpeed, .format = asn1_format_CHOICE_icao_as_json, .label = "wind_speed" },
	{ .type = &asn_DEF_ADSv2WindSpeedKmh, .format = asn1_format_ADSv2WindSpeedKmh_as_json, .label = "wind_speed" },
	{ .type = &asn_DEF_ADSv2WindSpeedKts, .format = asn1_format_ADSv2WindSpeedKts_as_json, .label = "wind_speed" },
	{ .type = &asn_DEF_ATSUListHiPrio, .format = la_asn1_format_any_as_string_as_json, .label = "high_priority" },
	{ .type = &asn_DEF_ATSUListMedPrio, .format = la_asn1_format_any_as_string_as_json, .label = "medium_priority" },
	{ .type = &asn_DEF_ATSUListLoPrio, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "low_priority" },
	{ .type = &asn_DEF_AirVector, .format = asn1_format_SEQUENCE_icao_as_json, .label = "air_vector" },
	{ .type = &asn_DEF_AirVectorModulus, .format = la_asn1_format_long_as_json, .label = "air_vector_modulus" },
	{ .type = &asn_DEF_Airspeed, .format = asn1_format_CHOICE_icao_as_json, .label = "airspeed" },
	{ .type = &asn_DEF_AirspeedChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_airspeed_changes" },
	{ .type = &asn_DEF_AirspeedChangeTolerance, .format = asn1_format_SEQUENCE_icao_as_json, .label = "airspeed" },
	{ .type = &asn_DEF_AirspeedRangeChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_airspeed_range_changes" },
	{ .type = &asn_DEF_ClimbSpeed, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "climb_speed" },
	{ .type = &asn_DEF_ConnectedATSUList, .format = asn1_format_SEQUENCE_icao_as_json, .label = "connected_atsu_list" },
	{ .type = &asn_DEF_ContractDetailsNotSupporting, .format = asn1_format_CHOICE_icao_as_json, .label = "contract_details_not_supporting" },
	{ .type = &asn_DEF_ContractNumber, .format = la_asn1_format_long_as_json, .label = "contract_number" },
	{ .type = &asn_DEF_DCRAirVector, .format = la_asn1_format_label_only_as_json, .label = "report_air_vector" },
	{ .type = &asn_DEF_DCRGroundVector, .format = la_asn1_format_label_only_as_json, .label = "report_ground_vector" },
	{ .type = &asn_DEF_DCRPlannedFinalApproachSpeed, .format = la_asn1_format_label_only_as_json, .label = "report_planned_final_approach_speed" },
	{ .type = &asn_DEF_DCRProjectedProfile, .format = la_asn1_format_label_only_as_json, .label = "report_projected_profile" },
	{ .type = &asn_DEF_DCRRNPProfile, .format = la_asn1_format_label_only_as_json, .label = "report_rnp_profile" },
	{ .type = &asn_DEF_DCRSpeedScheduleProfile, .format = la_asn1_format_label_only_as_json, .label = "report_speed_schedule_profile" },
	{ .type = &asn_DEF_DemandContractRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_demand_contract_request" },
	{ .type = &asn_DEF_DemandReport, .format = asn1_format_SEQUENCE_icao_as_json, .label = "on_demand_report" },
	{ .type = &asn_DEF_ECRRNPNotMet, .format = la_asn1_format_label_only_as_json, .label = "report_when_rnp_not_met" },
	{ .type = &asn_DEF_ECRRTAStatusChange, .format = la_asn1_format_label_only_as_json, .label = "report_rta_status_changes" },
	{ .type = &asn_DEF_ECRWaypointChange, .format = la_asn1_format_label_only_as_json, .label = "report_waypoint_changes" },
	{ .type = &asn_DEF_DescentSpeed, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "descent_speed" },
	{ .type = &asn_DEF_EPPEventChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_epp_changes" },
	{ .type = &asn_DEF_EPPFlightPlanChangeRequest, .format = la_asn1_format_label_only_as_json, .label = "report_epp_flight_plan_changes" },
	{ .type = &asn_DEF_EPPLevel, .format = asn1_format_CHOICE_icao_as_json, .label = "epp_level" },
	{ .type = &asn_DEF_EPPLimitations, .format = asn1_format_EPPLimitations_as_json, .label = "epp_limitations" },
	{ .type = &asn_DEF_EPPNextWptInHorizonRequest, .format = la_asn1_format_label_only_as_json, .label = "report_next_wpt_in_horizon" },
	{ .type = &asn_DEF_EPPTolGCDistance, .format = asn1_format_EPPTolGCDistance_as_json, .label = "great_circle_distance" },
	{ .type = &asn_DEF_EPPTolLevel, .format = asn1_format_LevelFeet_as_json, .label = "alt" },
	{ .type = &asn_DEF_EPPTolETA, .format = asn1_format_EPPTolETA_as_json, .label = "eta" },
	{ .type = &asn_DEF_EPPToleranceChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_epp_tolerance_changes" },
	{ .type = &asn_DEF_EPPTolerancesValues, .format = asn1_format_SEQUENCE_icao_as_json, .label = "epp_tolerances" },
	{ .type = &asn_DEF_EPPNumWaypoints, .format = la_asn1_format_long_as_json, .label = "number_of_waypoints" },
	{ .type = &asn_DEF_EPPTimeInterval, .format = asn1_format_EPPTimeInterval_as_json, .label = "time_interval" },
	{ .type = &asn_DEF_EPPRequest, .format = asn1_format_CHOICE_icao_as_json, .label = "report_extended_projected_profile" },
	{ .type = &asn_DEF_EPPWindow, .format = asn1_format_CHOICE_icao_as_json, .label = "epp_window" },
	{ .type = &asn_DEF_EPUChangeTolerance, .format = asn1_format_EPUChangeTolerance_as_json, .label = "report_fom_changes_exceeding" },
	{ .type = &asn_DEF_ETA, .format = asn1_format_SEQUENCE_icao_as_json, .label = "eta" },
	{ .type = &asn_DEF_EstimatedPositionUncertainty, .format = asn1_format_EstimatedPositionUncertainty_as_json, .label = "estimated_position_uncertainty" },
	{ .type = &asn_DEF_EventContractRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_event_contract_request" },
	{ .type = &asn_DEF_EventReport, .format = asn1_format_SEQUENCE_icao_as_json, .label = "event_report" },
	{ .type = &asn_DEF_EventTypeNotSupported, .format = asn1_format_EventTypeNotSupported_as_json, .label = "unsupported_events" },
	{ .type = &asn_DEF_EventTypeReported, .format = la_asn1_format_ENUM_as_json, .label = "reported_event" },
	{ .type = &asn_DEF_ExtendedProjectedProfile, .format = asn1_format_SEQUENCE_icao_as_json, .label = "extended_projected_profile" },
	{ .type = &asn_DEF_ExtendedProjectedProfileModulus, .format = asn1_format_SEQUENCE_icao_as_json, .label = "extended_projected_profile_modulus" },
	{ .type = &asn_DEF_ExtendedWayPointSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "extended_waypoint_sequence" },
	{ .type = &asn_DEF_ExtendedWayPointSequenceElement, .format = asn1_format_SEQUENCE_icao_as_json, .label = "waypoint_data" },
	{ .type = &asn_DEF_FigureOfMerit, .format = asn1_format_SEQUENCE_icao_as_json, .label = "figure_of_merit" },
	{ .type = &asn_DEF_FinalApproachSpeedChange, .format = asn1_format_SpeedIndicated_as_json, .label = "report_planned_final_approach_speed_changes" },
	{ .type = &asn_DEF_FinalCruiseSpeedAtToD, .format = asn1_format_SEQUENCE_icao_as_json, .label = "final_cruise_speed_at_top_of_descent" },
	{ .type = &asn_DEF_GrossMass, .format = asn1_format_GrossMass_as_json, .label = "gross_mass" },
	{ .type = &asn_DEF_GroundSpeed, .format = asn1_format_GroundSpeed_as_json, .label = "ground_speed" },
	{ .type = &asn_DEF_GroundSpeedChange, .format = asn1_format_SpeedIndicated_as_json, .label = "report_ground_speed_changes" },
	{ .type = &asn_DEF_GroundTrack, .format = asn1_format_GroundTrack_as_json, .label = "ground_track" },
	{ .type = &asn_DEF_GroundVector, .format = asn1_format_SEQUENCE_icao_as_json, .label = "ground_vector" },
	{ .type = &asn_DEF_GroundVectorModulus, .format = la_asn1_format_long_as_json, .label = "ground_vector_modulus" },
	{ .type = &asn_DEF_Heading, .format = asn1_format_GroundTrack_as_json, .label = "heading" },
	{ .type = &asn_DEF_Ias, .format = asn1_format_SpeedIndicated_as_json, .label = "ias" },
	{ .type = &asn_DEF_IasTolerance, .format = asn1_format_SpeedIndicated_as_json, .label = "ias" },
	{ .type = &asn_DEF_IasChange, .format = asn1_format_SpeedIndicated_as_json, .label = "ias_change" },
	{ .type = &asn_DEF_InitialCruiseSpeedAtToC, .format = asn1_format_SEQUENCE_icao_as_json, .label = "initial_cruise_speed_at_top_of_climb" },
	{ .type = &asn_DEF_LateralFlightManaged, .format = la_asn1_format_bool_as_json, .label = "lateral_flight_managed" },
	{ .type = &asn_DEF_LateralDeviationChange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_lateral_deviation_changes" },
	{ .type = &asn_DEF_LateralDeviationOffsetTag, .format = la_asn1_format_label_only_as_json, .label = "offset_tag" /* ? */ },
	{ .type = &asn_DEF_LateralDeviationThresholdLeft, .format = asn1_format_LateralDeviationThreshold_as_json, .label = "left_threshold" },
	{ .type = &asn_DEF_LateralDeviationThresholdRight, .format = asn1_format_LateralDeviationThreshold_as_json, .label = "right_threshold" },
	{ .type = &asn_DEF_LateralType, .format = asn1_format_SEQUENCE_icao_as_json, .label = "lateral_type" },
	{ .type = &asn_DEF_LateralTypeFlyby, .format = asn1_format_CHOICE_icao_as_json, .label = "fly_by" },
	{ .type = &asn_DEF_LateralTypeFixedRadiusTransition, .format = asn1_format_CHOICE_icao_as_json, .label = "fixed_radius_transition" },
	{ .type = &asn_DEF_LateralTypeOffsetStart, .format = la_asn1_format_label_only_as_json, .label = "offset_start" },
	{ .type = &asn_DEF_LateralTypeOffsetReached, .format = la_asn1_format_label_only_as_json, .label = "offset_reached" },
	{ .type = &asn_DEF_LateralTypeReturnToParentPathInitiation, .format = la_asn1_format_label_only_as_json, .label = "return_to_parent_path_initiation" },
	{ .type = &asn_DEF_LateralTypeOffsetEnd, .format = la_asn1_format_label_only_as_json, .label = "offset_end" },
	{ .type = &asn_DEF_LateralTypeOffset, .format = la_asn1_format_label_only_as_json, .label = "offset" },
	{ .type = &asn_DEF_LateralTypeOverfly, .format = la_asn1_format_label_only_as_json, .label = "overfly" },
	{ .type = &asn_DEF_LateralTypeFlightPlanWayPoint, .format = la_asn1_format_label_only_as_json, .label = "flight_plan_waypoint" },
	{ .type = &asn_DEF_LateralTypeFollowedByDisco, .format = la_asn1_format_label_only_as_json, .label = "followed_by_discontinuity" },
	{ .type = &asn_DEF_LevelChange, .format = asn1_format_LevelFeet_as_json, .label = "report_level_changes_exceeding" },
	{ .type = &asn_DEF_LevelConstraint, .format = asn1_format_CHOICE_icao_as_json, .label = "level_constraint" },
	{ .type = &asn_DEF_LevelConstraintQualifier, .format = la_asn1_format_ENUM_as_json, .label = "level_constraint_type" },
	{ .type = &asn_DEF_LevelRangeDeviation, .format = asn1_format_CHOICE_icao_as_json, .label = "report_level_range_deviation" },
	{ .type = &asn_DEF_LevelRangeDeviationBoth, .format = asn1_format_SEQUENCE_icao_as_json, .label = "level_range_deviation_both" },
	{ .type = &asn_DEF_LevelRangeDeviationCeiling, .format = asn1_format_LevelFeet_as_json, .label = "upper_limit" },
	{ .type = &asn_DEF_LevelRangeDeviationFloor, .format = asn1_format_LevelFeet_as_json, .label = "lower_limit" },
	{ .type = &asn_DEF_MachAndIas, .format = asn1_format_SEQUENCE_icao_as_json, .label = "mach_and_ias" },
	{ .type = &asn_DEF_MachNumberChange, .format = asn1_format_SpeedMach_as_json, .label = "mach_number_change" },
	{ .type = &asn_DEF_MachNumberTolerance, .format = asn1_format_MachNumberTolerance_as_json, .label = "mach_number" },
	{ .type = &asn_DEF_MetInfo, .format = asn1_format_SEQUENCE_icao_as_json, .label = "meteo_data" },
	{ .type = &asn_DEF_MinMaxIAS, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "min_max_ias" },
	{ .type = &asn_DEF_MinMaxMach, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "min_max_mach" },
	{ .type = &asn_DEF_MinMaxSpeed, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "min_max_speed" },
	{ .type = &asn_DEF_MetInfoModulus, .format = asn1_format_SEQUENCE_icao_as_json, .label = "meteo_info_modulus" },
	{ .type = &asn_DEF_MetInfoRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "meteo_info_modulus" },
	{ .type = &asn_DEF_Modulus, .format = la_asn1_format_long_as_json, .label = "reporting_frequency" },
	{ .type = &asn_DEF_MSLAltitude, .format = asn1_format_LevelFeet_as_json, .label = "alt_msl" },
	{ .type = &asn_DEF_MultipleNavigationalUnitsOperating, .format = la_asn1_format_bool_as_json, .label = "multiple_nav_units_operating" },
	{ .type = &asn_DEF_NominalSpeed, .format = asn1_format_CHOICE_icao_as_json, .label = "nominal_speed" },
	{ .type = &asn_DEF_PeriodicContractRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "adsc_periodic_contract_request" },
	{ .type = &asn_DEF_PeriodicReport, .format = asn1_format_SEQUENCE_icao_as_json, .label = "periodic_report" },
	{ .type = &asn_DEF_PlannedFinalAppSpeedModulus, .format = la_asn1_format_long_as_json, .label = "planned_final_approach_speed_modulus" },
	{ .type = &asn_DEF_PredictedGrossMassAtToD, .format = asn1_format_GrossMass_as_json, .label = "predicted_gross_mass_at_top_of_descent" },
	{ .type = &asn_DEF_ProjectedProfile, .format = asn1_format_SEQUENCE_icao_as_json, .label = "projected_profile" },
	{ .type = &asn_DEF_ProjectedProfileModulus, .format = la_asn1_format_long_as_json, .label = "projected_profile_modulus" },
	{ .type = &asn_DEF_QNEAltitude, .format = asn1_format_LevelFeet_as_json, .label = "alt_qne" },
	{ .type = &asn_DEF_QNHAltitude, .format = asn1_format_SEQUENCE_icao_as_json, .label = "alt_qnh" },
	{ .type = &asn_DEF_RejectDetails, .format = asn1_format_RejectDetails_as_json, .label = "reject_reason" },
	{ .type = &asn_DEF_RNPProfile, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "rnp_profile" },
	{ .type = &asn_DEF_RNPProfileModulus, .format = la_asn1_format_long_as_json, .label = "rnp_profile_modulus" },
	{ .type = &asn_DEF_RNPSegment, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rnp_segment" },
	{ .type = &asn_DEF_RNPSegmentEndPoint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "end" },
	{ .type = &asn_DEF_RNPSegmentStartPoint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "start" },
	{ .type = &asn_DEF_RNPValue, .format = asn1_format_RNPValue_as_json, .label = "rnp_value" },
	{ .type = &asn_DEF_RTA, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rta" },
	{ .type = &asn_DEF_RTASecTolerance, .format = asn1_format_RTASecTolerance_as_json, .label = "rta_sec_tolerance" },
	{ .type = &asn_DEF_RTAStatus, .format = la_asn1_format_ENUM_as_json, .label = "rta_status" },
	{ .type = &asn_DEF_RTAStatusData, .format = asn1_format_SEQUENCE_icao_as_json, .label = "rta_status_data" },
	{ .type = &asn_DEF_RTAType, .format = la_asn1_format_ENUM_as_json, .label = "rta_type" },
	{ .type = &asn_DEF_ReportTypeAndPeriodNotSupported, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_type_and_period_not_supported" },
	{ .type = &asn_DEF_ReportTypeNotSupported, .format = asn1_format_ReportTypeNotSupported_as_json, .label = "unsupported_reports" },
	{ .type = &asn_DEF_ReportingRate, .format = asn1_format_ReportingRate_as_json, .label = "reporting_rate" },
	{ .type = &asn_DEF_SingleLevel, .format = asn1_format_SEQUENCE_icao_as_json, .label = "single_level" },
	{ .type = &asn_DEF_SingleLevelSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "single_level_sequence" },
	{ .type = &asn_DEF_SpeedConstraint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "speed_constraint" },
	{ .type = &asn_DEF_SpeedIASMach, .format = asn1_format_CHOICE_icao_as_json, .label = "speed_ias_mach" },
	{ .type = &asn_DEF_SpeedManaged, .format = la_asn1_format_bool_as_json, .label = "speed_managed" },
	{ .type = &asn_DEF_SpeedQualifier, .format = la_asn1_format_ENUM_as_json, .label = "type" },
	{ .type = &asn_DEF_SpeedScheduleBlock, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "block_speed_schedule" },
	{ .type = &asn_DEF_SpeedScheduleProfile, .format = asn1_format_SEQUENCE_icao_as_json, .label = "speed_schedule_profile" },
	{ .type = &asn_DEF_SpeedScheduleProfileModulus, .format = la_asn1_format_long_as_json, .label = "speed_schedule_profile_modulus" },
	{ .type = &asn_DEF_SpeedScheduleSingle, .format = asn1_format_SEQUENCE_icao_as_json, .label = "single_speed_schedule" },
	{ .type = &asn_DEF_TimeManaged, .format = la_asn1_format_bool_as_json, .label = "time_managed" },
	{ .type = &asn_DEF_TOAComputationTime, .format = asn1_format_SEQUENCE_icao_as_json, .label = "computation_time" },
	{ .type = &asn_DEF_TOARange, .format = asn1_format_SEQUENCE_icao_as_json, .label = "toa_range" },
	{ .type = &asn_DEF_TOARangeEarliestETA, .format = asn1_format_SEQUENCE_icao_as_json, .label = "eta_earliest" },
	{ .type = &asn_DEF_TOARangeLatestETA, .format = asn1_format_SEQUENCE_icao_as_json, .label = "eta_latest" },
	{ .type = &asn_DEF_TOARangeRequest, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_toa_range" },
	{ .type = &asn_DEF_TOARangeRequestModulus, .format = asn1_format_SEQUENCE_icao_as_json, .label = "toa_range_modulus" },
	{ .type = &asn_DEF_ThreeDPosition, .format = asn1_format_SEQUENCE_icao_as_json, .label = "position" },
	{ .type = &asn_DEF_Timesec, .format = asn1_format_SEQUENCE_icao_as_json, .label = "time" },
	{ .type = &asn_DEF_TrajectoryIntentStatus, .format = asn1_format_SEQUENCE_icao_as_json, .label = "trajectory_intent_status" },
	{ .type = &asn_DEF_TurbulenceDeviation, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_turbulence_deviation" },
	{ .type = &asn_DEF_TurbulenceEDRAverage, .format = asn1_format_TurbulenceEDRValue_as_json, .label = "average_edr_value" },
	{ .type = &asn_DEF_TurbulenceEDRPeak, .format = asn1_format_SEQUENCE_icao_as_json, .label = "peak_edr_value" },
	{ .type = &asn_DEF_TurbulenceEDRValue, .format = asn1_format_TurbulenceEDRValue_as_json, .label = "edr_value" },
	{ .type = &asn_DEF_TurbulenceMinutesInPast, .format = asn1_format_TurbulenceMinutesInThePast_as_json, .label = "time_ago" },
	{ .type = &asn_DEF_TurbulenceObservationWindow, .format = asn1_format_TurbulenceObservationWindow_as_json, .label = "observation_window" },
	{ .type = &asn_DEF_TurbulencePeakThreshold, .format = asn1_format_TurbulenceEDRValue_as_json, .label = "peak_edr_threshold" },
	{ .type = &asn_DEF_TurnRadius, .format = asn1_format_TurnRadius_as_json, .label = "turn_radius" },
	{ .type = &asn_DEF_TurnRadiusNotAvailable, .format = la_asn1_format_label_only_as_json, .label = "turn_radius_not_available" },
	{ .type = &asn_DEF_VerticalClearanceDeviation, .format = asn1_format_LevelFeet_as_json, .label = "report_vertical_clearance_deviation_exceeding" },
	{ .type = &asn_DEF_VerticalFlightManaged, .format = la_asn1_format_bool_as_json, .label = "vertical_flight_managed" },
	{ .type = &asn_DEF_VerticalRateDeviation, .format = asn1_format_SEQUENCE_icao_as_json, .label = "report_vertical_rate_deviation" },
	{ .type = &asn_DEF_VerticalRateDeviationLower, .format = asn1_format_VerticalRateEnglish_as_json, .label = "lower_limit" },
	{ .type = &asn_DEF_VerticalRateDeviationUpper, .format = asn1_format_VerticalRateEnglish_as_json, .label = "upper_limit" },
	{ .type = &asn_DEF_VerticalType, .format = asn1_format_VerticalType_as_json, .label = "vertical_type" },
	{ .type = &asn_DEF_Waypoint, .format = asn1_format_SEQUENCE_icao_as_json, .label = "waypoint" },
	{ .type = &asn_DEF_WaypointName, .format = la_asn1_format_any_as_string_as_json, .label = "waypoint_name" },
	{ .type = &asn_DEF_WayPointSequence, .format = asn1_format_SEQUENCE_OF_icao_as_json, .label = "waypoint_sequence" },
	{ .type = &asn_DEF_WayPointSequenceElement, .format = asn1_format_SEQUENCE_icao_as_json, .label = "waypoint_sequence_element" },
	{ .type = &asn_DEF_WindErrorModelUsed, .format = la_asn1_format_ENUM_as_json, .label = "wind_error_model_used" },
	{ .type = &asn_DEF_WindQualityFlag, .format = la_asn1_format_ENUM_as_json, .label = "wind_quality_flag" },
};

size_t asn1_icao_formatter_table_json_len = sizeof(asn1_icao_formatter_table_json) / sizeof(la_asn1_formatter);

la_asn1_formatter const asn1_acse_formatter_table_json[] = {
	{ .type = &asn_DEF_AARE_apdu, .format = asn1_format_SEQUENCE_acse_as_json, .label = "assoc_response" },
	{ .type = &asn_DEF_AARQ_apdu, .format = asn1_format_SEQUENCE_acse_as_json, .label = "assoc_request" },
	{ .type = &asn_DEF_ABRT_apdu, .format = asn1_format_SEQUENCE_acse_as_json, .label = "abort" },
	{ .type = &asn_DEF_ABRT_diagnostic, .format = la_asn1_format_ENUM_as_json, .label = "abort_diagnostics" },
	{ .type = &asn_DEF_ABRT_source  , .format = asn1_format_ABRT_source_as_json, .label = "abort_source" },
	{ .type = &asn_DEF_ACSE_apdu, .format = asn1_format_CHOICE_acse_as_json, .label = "acse_apdu" },
	{ .type = &asn_DEF_AE_qualifier, .format = asn1_format_CHOICE_acse_as_json, .label = "ae_qualifier" },
	{ .type = &asn_DEF_AE_qualifier_form2, .format = la_asn1_format_long_as_json, .label = "ae_qualifier_form2" },
	{ .type = &asn_DEF_AP_title, .format = asn1_format_CHOICE_acse_as_json, .label = "ap_title" },
	{ .type = &asn_DEF_AP_title_form2, .format = asn1_format_OBJECT_IDENTIFIER_as_json, .label = "ap_title_form2" },
	{ .type = &asn_DEF_Application_context_name, .format = asn1_format_OBJECT_IDENTIFIER_as_json, .label = "app_ctx_name" },
	{ .type = &asn_DEF_Associate_result, .format = asn1_format_Associate_result_as_json, .label = "assoc_result" },
	{ .type = &asn_DEF_Release_request_reason, .format = asn1_format_Release_request_reason_as_json, .label = "reason" },
	{ .type = &asn_DEF_Release_response_reason, .format = asn1_format_Release_response_reason_as_json, .label = "reason" },
	{ .type = &asn_DEF_RLRE_apdu, .format = asn1_format_SEQUENCE_acse_as_json, .label = "release_response" },
	{ .type = &asn_DEF_RLRQ_apdu, .format = asn1_format_SEQUENCE_acse_as_json, .label = "release_request" },
	// Supported in ATN ULCS, but not included in JSON output
	{ .type = &asn_DEF_ACSE_requirements, .format = NULL, .label = NULL },
	{ .type = &asn_DEF_Associate_source_diagnostic, .format = NULL, .label = NULL },
	{ .type = &asn_DEF_Association_information, .format = NULL, .label = NULL },
	{ .type = &asn_DEF_Authentication_value, .format = NULL, .label = NULL }
	// Not supported in ATN ULCS
	// { .type = &asn_DEF_AE_invocation_identifier, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_AE_qualifier_form1, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_AP_invocation_identifier, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_AP_title_form1, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_Application_context_name_list, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_AttributeTypeAndValue, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_EXTERNALt, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_Implementation_data, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_Mechanism_name, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_Name, .format = NULL, .label = NULL },
	// { .type = &asn_DEF_RDNSequence , .format = NULL, .label = NULL },
	// { .type = &asn_DEF_RelativeDistinguishedName, .format = NULL, .label = NULL },
};

size_t asn1_acse_formatter_table_json_len = sizeof(asn1_acse_formatter_table_json) / sizeof(la_asn1_formatter);
