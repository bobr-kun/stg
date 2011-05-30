/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "SMUX"
 * 	found in "SMUX.asn1"
 * 	`asn1c -fskeletons-copy`
 */

#include <asn_internal.h>

#include "RReqPDU.h"

static int
memb_priority_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= -1 && value <= 2147483647)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_RReqPDU_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct RReqPDU, subtree),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_ObjectName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subtree"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RReqPDU, priority),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		memb_priority_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"priority"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RReqPDU, operation),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"operation"
		},
};
static ber_tlv_tag_t asn_DEF_RReqPDU_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_RReqPDU_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, 0, 1 }, /* priority at 96 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 2, -1, 0 }, /* operation at 101 */
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 } /* subtree at 93 */
};
static asn_SEQUENCE_specifics_t asn_SPC_RReqPDU_specs_1 = {
	sizeof(struct RReqPDU),
	offsetof(struct RReqPDU, _asn_ctx),
	asn_MAP_RReqPDU_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_RReqPDU = {
	"RReqPDU",
	"RReqPDU",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RReqPDU_tags_1,
	sizeof(asn_DEF_RReqPDU_tags_1)
		/sizeof(asn_DEF_RReqPDU_tags_1[0]) - 1, /* 1 */
	asn_DEF_RReqPDU_tags_1,	/* Same as above */
	sizeof(asn_DEF_RReqPDU_tags_1)
		/sizeof(asn_DEF_RReqPDU_tags_1[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_RReqPDU_1,
	3,	/* Elements count */
	&asn_SPC_RReqPDU_specs_1	/* Additional specs */
};
