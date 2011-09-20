/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "RFC1213-MIB"
 * 	found in "RFC1213-MIB.asn1"
 * 	`asn1c -fskeletons-copy`
 */

#ifndef	_IpNetToMediaEntry_H_
#define	_IpNetToMediaEntry_H_


#include <asn_application.h>

/* Including external dependencies */
#include <INTEGER.h>
#include "PhysAddress.h"
#include "IpAddress.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IpNetToMediaEntry */
typedef struct IpNetToMediaEntry {
	INTEGER_t	 ipNetToMediaIfIndex;
	PhysAddress_t	 ipNetToMediaPhysAddress;
	IpAddress_t	 ipNetToMediaNetAddress;
	INTEGER_t	 ipNetToMediaType;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} IpNetToMediaEntry_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_IpNetToMediaEntry;

#ifdef __cplusplus
}
#endif

#endif	/* _IpNetToMediaEntry_H_ */