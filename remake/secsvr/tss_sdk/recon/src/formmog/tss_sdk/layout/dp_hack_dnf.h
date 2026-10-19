// Canonical layout of the tss_dpsdk DNF sub-command packet structs.
//
// ORIG evidence: audit/tu_report.py --type '<name>' and the DWARF definition
// DIEs in CU dp_property_trans_hacker.cpp / dp_mail_verify_hacker.cpp.
// All of them are packed (the DIE offsets show no 4-byte padding).
//
//   DPHackDnfCmdHead       13   type_ char @0, cmd_id_ short @1,
//                               length_ int @3, crc_ int @7,
//                               sequence_ unsigned short @11
//   DPHackDnfCmdEnchant    14   header_ @0, flag_ unsigned char @13
//   DPHackDnfCmdRequest    16   header_ @0, role_uid_ u16 @13, sub_id_ u8 @15
//   DPHackDnfCmdUid        26   header_ @0, pad_[11] @13, role_uid_ u16 @24
//   DPHackDnfItemMove      25   header_ @0, flag_ u8 @13, pad_[10] @14,
//                               sub_id_ u8 @24
//   DPHackDnfMailHead      18   header_ @0, role_name_len_ u32 @13,
//                               role_name_ u8[1] @17
//   DPHackDnfMailItem      10   item_posi_ u16 @0, item_id_ u32 @2,
//                               item_num_ u32 @6
//   DPHackDnfMailBody      16   gold_num_ u32 @0, items_num_ u8 @4,
//                               anonymous union @5 (11 bytes)
//   DPHackDnfTradeConfirm  14   header_ @0, sub_id_ u8 @13
//   DPHackStoreResponse    18   header_ @0, pad_[3] @13, role_uid_ u16 @16
//
// DPHackDnfMailBody: the DWARF union member is unnamed (model path `@2`) and
// both alternatives are anonymous 11-byte/10-byte structs, so the union is
// declared anonymous here exactly as ORIG emits it.  The `items_[1]` and
// `item_` names are then injected into the enclosing struct.
#ifndef TSS_SDK_LAYOUT_DP_HACK_DNF_H_
#define TSS_SDK_LAYOUT_DP_HACK_DNF_H_

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_dpsdk {

struct DPHackDnfCmdHead {
    char type_;                  // +0
    unsigned short cmd_id_;      // +1
    int length_;                 // +3
    int crc_;                    // +7
    unsigned short sequence_;    // +11
} __attribute__((packed));

struct DPHackDnfCmdUid {
    DPHackDnfCmdHead header_;    // +0
    unsigned char pad_[11];      // +13
    unsigned short role_uid_;    // +24
} __attribute__((packed));

struct DPHackDnfCmdRequest {
    DPHackDnfCmdHead header_;    // +0
    unsigned short role_uid_;    // +13
    unsigned char sub_id_;       // +15
} __attribute__((packed));

struct DPHackDnfTradeConfirm {
    DPHackDnfCmdHead header_;    // +0
    unsigned char sub_id_;       // +13
} __attribute__((packed));

struct DPHackDnfItemMove {
    DPHackDnfCmdHead header_;    // +0
    unsigned char flag_;         // +13
    unsigned char pad_[10];      // +14
    unsigned char sub_id_;       // +24
} __attribute__((packed));

struct DPHackDnfCmdEnchant {
    DPHackDnfCmdHead header_;    // +0
    unsigned char flag_;         // +13
} __attribute__((packed));

struct DPHackStoreResponse {
    DPHackDnfCmdHead header_;    // +0
    unsigned char pad_[3];       // +13
    unsigned short role_uid_;    // +16
} __attribute__((packed));

struct DPHackDnfMailItem {
    unsigned short item_posi_;   // +0
    unsigned int item_id_;       // +2
    unsigned int item_num_;      // +6
} __attribute__((packed));

// ORIG emits both the array element and the scalar alternative of the
// DPHackDnfMailBody union as anonymous 11-byte structs; this named form is
// the one the reconstruction already used and keeps the same bytes.
struct DPHackDnfMailItemWrap {
    unsigned char pad_;              // +0
    DPHackDnfMailItem item_;         // +1
} __attribute__((packed));

struct DPHackDnfMailHead {
    DPHackDnfCmdHead header_;        // +0
    unsigned int role_name_len_;     // +13
    unsigned char role_name_[1];     // +17
} __attribute__((packed));

struct DPHackDnfMailBody {
    unsigned int gold_num_;          // +0
    unsigned char items_num_;        // +4
    union {                          // +5, unnamed in ORIG
        DPHackDnfMailItemWrap items_[1];
        DPHackDnfMailItem item_;
    };
} __attribute__((packed));

TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfCmdHead) == 13,
                      dphackdnfcmdhead_size_must_be_13);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfCmdHead, sequence_) == 11,
                      dphackdnfcmdhead_sequence_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfCmdEnchant) == 14,
                      dphackdnfcmdenchant_size_must_be_14);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfCmdRequest) == 16,
                      dphackdnfcmdrequest_size_must_be_16);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfCmdUid) == 26,
                      dphackdnfcmduid_size_must_be_26);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfCmdUid, role_uid_) == 24,
                      dphackdnfcmduid_role_uid_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfItemMove) == 25,
                      dphackdnfitemmove_size_must_be_25);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfItemMove, sub_id_) == 24,
                      dphackdnfitemmove_sub_id_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfMailItem) == 10,
                      dphackdnfmailitem_size_must_be_10);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfMailItemWrap) == 11,
                      dphackdnfmailitemwrap_size_must_be_11);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfMailHead) == 18,
                      dphackdnfmailhead_size_must_be_18);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfMailHead, role_name_) == 17,
                      dphackdnfmailhead_role_name_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfMailBody) == 16,
                      dphackdnfmailbody_size_must_be_16);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfMailBody, items_) == 5,
                      dphackdnfmailbody_items_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackDnfMailBody, item_) == 5,
                      dphackdnfmailbody_item_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDnfTradeConfirm) == 14,
                      dphackdnftradeconfirm_size_must_be_14);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackStoreResponse) == 18,
                      dphackstoreresponse_size_must_be_18);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackStoreResponse, role_uid_) == 16,
                      dphackstoreresponse_role_uid_offset);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_HACK_DNF_H_
