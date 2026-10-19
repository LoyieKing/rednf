// tss_dp_comm configuration structures of formmog_conf_tsssdk_dp.cpp
// (libtss_sdk.so.1.0.394, tag 394) - TDR-generated XML/loading support.
//
// Every class, member, member order and member type below is the CU's own
// DWARF (cu_layout.py formmog_conf_tsssdk_dp.cpp): the byte sizes are exactly
// the DW_AT_byte_size values of the ORIG DIEs (DPGameCmdStruct 2,
// DPValueArray_8 34, DPValueArray_128 514, DPCmdLists 2064, DPSubCmdList 42,
// DPHackFragment 16, DPHackFilter 2050, DPLogConf 296, DPTimeConfig 28,
// DPConfigBase 6, DPProtoConfig 10, DPCaptchaMethod 16, DPBanMode 130,
// DPDropCmd 42, DPDelayCmd 35, DPPropertyCmd 48, DPCodeInfo 14884,
// DPCmdHack 7100, DPConfig 22842), and those sizes only hold with 1-byte
// packing (the ORIG places uint32_t arrays at odd offsets, e.g.
// DPValueArray_8::values at +2).
//
// The 12 member functions of each class are the 228 DWARF-attributed symbols
// of the CU (cu_symbols.py).
#ifndef TSS_DP_COMM_FORMMOG_CONF_TSSSDK_DP_H_
#define TSS_DP_COMM_FORMMOG_CONF_TSSSDK_DP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tsf4g_tdr_compat_cfg.h"

namespace tss_dp_comm {

#pragma pack(push, 1)

class DPValueArray_8;

class DPGameCmdStruct {
public:
    uint8_t cmd_id_type;    // +0
    uint8_t cmd_id_offset;  // +1

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPValueArray_8 {
public:
    uint16_t length;        // +0
    uint32_t values[8];     // +2

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPValueArray_128 {
public:
    uint16_t length;        // +0
    uint32_t values[128];   // +2

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPCmdLists {
public:
    uint16_t cur_posi;              // +0
    uint16_t lists_count;           // +2
    uint32_t cur_crc;               // +4
    DPValueArray_128 cmd_lists[4];  // +8

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPSubCmdList {
public:
    uint8_t is_accept;           // +0
    uint8_t cmd_id_type;         // +1
    DPGameCmdStruct dp_subcmd_hack;  // +2
    uint32_t cmd_id;             // +4
    DPValueArray_8 subcmd_list;  // +8

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPHackFragment {
public:
    uint32_t cmd_start;     // +0
    uint32_t cmd_end;       // +4
    uint8_t hack_list[8];   // +8

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPHackFilter {
public:
    uint16_t fragment_count;            // +0
    DPHackFragment hack_fragments[128]; // +2

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPLogConf {
public:
    uint32_t bill_level;        // +0
    uint32_t game_id;           // +4
    char bill_path[256];        // +8
    char monitor_address[32];   // +264

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPTimeConfig {
public:
    uint32_t max_init_pkg_timeout;        // +0
    uint32_t max_static_pkg_timeout;      // +4
    uint32_t max_confirm_pkg_timeout;     // +8
    uint32_t max_send_test_pkg_timeout;   // +12
    uint32_t check_time;                  // +16
    uint32_t send_bin_file_gap;           // +20
    uint32_t send_bin_code_gap;           // +24

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPConfigBase {
public:
    uint8_t is_use_dp;               // +0
    uint8_t is_restore_usr_space;    // +1
    uint8_t is_use_client_version;   // +2
    uint8_t disable_anticheat_pkg;   // +3
    uint8_t use_punish_notify;       // +4
    uint8_t dp_record_using_time;    // +5

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPProtoConfig {
public:
    uint16_t dp_fragmentation_size;         // +0
    uint16_t dp_max_encrypt_size;           // +2
    uint8_t use_sequence_check;             // +4
    uint8_t proto_check_switch;             // +5
    uint8_t seq_check_switch;               // +6
    uint8_t raw_check_switch;               // +7
    uint8_t dp_decrypt_fail_protect;        // +8
    uint8_t explicit_use_dynamic_encrypt;   // +9

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPCaptchaMethod {
public:
    uint32_t max_timeout_count;  // +0
    uint32_t min_try_count;      // +4
    uint32_t min_try_time;       // +8
    uint32_t waitting_response;  // +12

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPBanMode {
public:
    uint16_t length;        // +0
    uint32_t values[32];    // +2

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPDropCmd {
public:
    uint16_t length;        // +0
    uint32_t values[8];     // +2
    uint8_t actions[8];     // +34

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPDelayCmd {
public:
    uint8_t delay_cmd_time;         // +0
    DPValueArray_8 safemode_drop_cmdlist;  // +1

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPPropertyCmd {
public:
    uint32_t cmd_role_uid;             // +0
    uint32_t cmd_create_team;          // +4
    uint32_t cmd_drop_item;            // +8
    uint32_t cmd_create_role;          // +12
    uint32_t cmd_enter_private_store;  // +16
    uint32_t cmd_trade_accept;         // +20
    uint32_t cmd_trade_confirm;        // +24
    uint32_t cmd_trade_put_item;       // +28
    uint32_t cmd_enchant_put_item;     // +32
    uint32_t cmd_enchant_confirm;      // +36
    uint32_t cmd_simple_mail;          // +40
    uint32_t cmd_complex_mail;         // +44

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPCodeInfo {
public:
    uint16_t cmd_list_buf_size;         // +0
    uint8_t cmd_list_buf[10240];        // +2
    DPCmdLists up_cmd_list;             // +10242
    DPCmdLists down_cmd_list;           // +12306
    uint16_t code_dir_count;            // +14370
    char code_dir_path[2][256];         // +14372

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPCaptchaMethod;

class DPCmdHack {
public:
    DPDelayCmd delay_cmd;                   // +0
    uint8_t safemode_to_client;             // +35
    uint8_t safemode_hint_client;           // +36
    uint8_t cf_room_qtalk_enable;           // +37
    DPHackFilter up_cmd_hack;               // +38
    DPHackFilter down_cmd_hack;             // +2088
    DPHackFilter raw_cmd_hack;              // +4138
    DPCaptchaMethod captch_method;          // +6188
    uint16_t drop_subcmd_count;             // +6204
    DPSubCmdList drop_subcmd_list[8];       // +6206
    uint16_t forward_subcmd_count;          // +6542
    DPSubCmdList forward_subcmd_list[8];    // +6544
    DPBanMode ban_mode;                     // +6880
    DPDropCmd drop_cmd;                     // +7010
    DPPropertyCmd property_cmd;             // +7052

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class DPConfig {
public:
    DPConfigBase dp_base_conf;          // +0
    DPProtoConfig dp_proto_conf;        // +6
    DPLogConf dp_log_conf;              // +16
    DPTimeConfig dp_time_confg;         // +312
    DPCodeInfo dp_code_info;            // +340
    DPValueArray_128 uin_monitor_list;  // +15224
    DPGameCmdStruct dp_up_cmd_hack;     // +15738
    DPGameCmdStruct dp_down_cmd_hack;   // +15740
    DPCmdHack cmd_hack;                 // +15742

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

#pragma pack(pop)

} // namespace tss_dp_comm

namespace tss_dp_comm {

// Compile-time ABI checks.  Every number below is a DW_AT_byte_size or
// DW_AT_data_member_location of this CU's own DWARF, so a layout regression of
// either a configuration structure or the XML runtime model fails the build
// instead of silently changing the generated code shape.
#define TDR_DP_ASSERT(name, cond) typedef char tdr_dp_assert_##name[(cond) ? 1 : -1]

TDR_DP_ASSERT(sizeof_DPGameCmdStruct, sizeof(DPGameCmdStruct) == 2);
TDR_DP_ASSERT(sizeof_DPValueArray_8, sizeof(DPValueArray_8) == 34);
TDR_DP_ASSERT(sizeof_DPValueArray_128, sizeof(DPValueArray_128) == 514);
TDR_DP_ASSERT(sizeof_DPCmdLists, sizeof(DPCmdLists) == 2064);
TDR_DP_ASSERT(sizeof_DPSubCmdList, sizeof(DPSubCmdList) == 42);
TDR_DP_ASSERT(sizeof_DPHackFragment, sizeof(DPHackFragment) == 16);
TDR_DP_ASSERT(sizeof_DPHackFilter, sizeof(DPHackFilter) == 2050);
TDR_DP_ASSERT(sizeof_DPLogConf, sizeof(DPLogConf) == 296);
TDR_DP_ASSERT(sizeof_DPTimeConfig, sizeof(DPTimeConfig) == 28);
TDR_DP_ASSERT(sizeof_DPConfigBase, sizeof(DPConfigBase) == 6);
TDR_DP_ASSERT(sizeof_DPProtoConfig, sizeof(DPProtoConfig) == 10);
TDR_DP_ASSERT(sizeof_DPCaptchaMethod, sizeof(DPCaptchaMethod) == 16);
TDR_DP_ASSERT(sizeof_DPBanMode, sizeof(DPBanMode) == 130);
TDR_DP_ASSERT(sizeof_DPDropCmd, sizeof(DPDropCmd) == 42);
TDR_DP_ASSERT(sizeof_DPDelayCmd, sizeof(DPDelayCmd) == 35);
TDR_DP_ASSERT(sizeof_DPPropertyCmd, sizeof(DPPropertyCmd) == 48);
TDR_DP_ASSERT(sizeof_DPCodeInfo, sizeof(DPCodeInfo) == 14884);
TDR_DP_ASSERT(sizeof_DPCmdHack, sizeof(DPCmdHack) == 7100);
TDR_DP_ASSERT(sizeof_DPConfig, sizeof(DPConfig) == 22842);

TDR_DP_ASSERT(off_DPValueArray_8_values, offsetof(DPValueArray_8, values) == 2);
TDR_DP_ASSERT(off_DPSubCmdList_subcmd_list, offsetof(DPSubCmdList, subcmd_list) == 8);
TDR_DP_ASSERT(off_DPDropCmd_actions, offsetof(DPDropCmd, actions) == 34);
TDR_DP_ASSERT(off_DPDelayCmd_safemode, offsetof(DPDelayCmd, safemode_drop_cmdlist) == 1);
TDR_DP_ASSERT(off_DPCodeInfo_up_cmd_list, offsetof(DPCodeInfo, up_cmd_list) == 10242);
TDR_DP_ASSERT(off_DPCodeInfo_down_cmd_list, offsetof(DPCodeInfo, down_cmd_list) == 12306);
TDR_DP_ASSERT(off_DPCmdHack_up_cmd_hack, offsetof(DPCmdHack, up_cmd_hack) == 38);
TDR_DP_ASSERT(off_DPCmdHack_drop_subcmd_list, offsetof(DPCmdHack, drop_subcmd_list) == 6206);
TDR_DP_ASSERT(off_DPCmdHack_property_cmd, offsetof(DPCmdHack, property_cmd) == 7052);
TDR_DP_ASSERT(off_DPConfig_log_conf, offsetof(DPConfig, dp_log_conf) == 16);
TDR_DP_ASSERT(off_DPConfig_code_info, offsetof(DPConfig, dp_code_info) == 340);
TDR_DP_ASSERT(off_DPConfig_cmd_hack, offsetof(DPConfig, cmd_hack) == 15742);

TDR_DP_ASSERT(sizeof_TdrXmlFile, sizeof(tsf4g_tdr::TdrXmlFile) == 8);
TDR_DP_ASSERT(sizeof_TdrOutStream, sizeof(tsf4g_tdr::TdrOutStream) == 24);
TDR_DP_ASSERT(sizeof_TdrXmlWriter, sizeof(tsf4g_tdr::TdrXmlWriter) == 76);
TDR_DP_ASSERT(sizeof_TdrXmlReader, sizeof(tsf4g_tdr::TdrXmlReader) == 65624);
TDR_DP_ASSERT(sizeof_xml_base, sizeof(rapidxml::xml_base<char>) == 20);
TDR_DP_ASSERT(sizeof_xml_node, sizeof(rapidxml::xml_node<char>) == 48);
TDR_DP_ASSERT(sizeof_memory_pool, sizeof(rapidxml::memory_pool<char>) == 65556);
TDR_DP_ASSERT(sizeof_xml_document, sizeof(rapidxml::xml_document<char>) == 65604);

#undef TDR_DP_ASSERT

} // namespace tss_dp_comm

#endif // TSS_DP_COMM_FORMMOG_CONF_TSSSDK_DP_H_
