// Reconstructed from secagent disassembly (behavior matched, see DibData::load).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_antibot_crypt_data.cpp

#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/commlib/zenlib/zen_key_value_config.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/framework/comm_stat_define.h"

#include <stdio.h>
#include <string.h>

// secagent_file_load.cpp（本 TU 直接声明，避免坏桩头文件）
extern int load_bin_file(const char *file_path, unsigned char *data, unsigned int *data_len);

// 错误码（与二进制一致）
#define DIB_ERROR_NULL_PARAM  0x493f1
#define DIB_ERROR_CFG_FAIL    0x49402
#define DIB_ERROR_ITEM_COUNT  0x49403
#define DIB_ERROR_FILE_LOAD   0x49404
#define DIB_ERROR_KEY_COUNT   0x49405

DibData::DibData() {
    memset(ver_, 0, sizeof(ver_));
    dib_item_count_ = 0;
    memset(dib_item_array_, 0, sizeof(dib_item_array_));
}

DibData::~DibData() {
}

int DibData::load(const char *cfg_dir) {
    char cfg_path[512];
    KeyValueConfig dib_cfg;
    int ret;
    char key_name[64];
    unsigned int ver_size;
    char key_id[64];
    char key_type[64];
    char key_file_name[64];
    char key_key_len[64];
    char value_file_name[64];
    char dib_path[512];
    char v_start[64];
    char f_start[64];
    char range_len[64];
    unsigned int i;
    unsigned int j;

    if (cfg_dir == NULL) {
        return DIB_ERROR_NULL_PARAM;
    }

    memset(cfg_path, 0, sizeof(cfg_path));
    snprintf(cfg_path, sizeof(cfg_path), "%s/dib.cfg", cfg_dir);
    ret = dib_cfg.load_from_file(cfg_path);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("dibdata load cfg file fail. %s", cfg_path);
        return DIB_ERROR_CFG_FAIL;
    }

    dib_cfg.get_str("VERSION", key_name, 0x40, "00000000000000000000000000000000");
    // VERSION 为十六进制串，逐对解码写入 ver_
    ver_size = strlen(key_name);
    if (ver_size > 0 && ver_size <= 0x20) {
        unsigned char ver_high = 0;
        unsigned int ver_idx = 0;
        for (i = 0; i < ver_size; i++) {
            unsigned char c = (unsigned char)key_name[i];
            unsigned char nibble;
            if (c >= '0' && c <= '9') {
                nibble = (unsigned char)(c - '0');
            } else if (c >= 'A' && c <= 'F') {
                nibble = (unsigned char)(c - 'A' + 10);
            } else if (c >= 'a' && c <= 'f') {
                nibble = (unsigned char)(c - 'a' + 10);
            } else {
                nibble = 0;
            }
            if (i & 1) {
                ver_[ver_idx] = (unsigned char)(ver_high + nibble);
                ver_idx++;
                ver_high = 0;
            } else {
                ver_high = (unsigned char)(nibble << 4);
            }
        }
    }

    dib_cfg.get_int("dib.count", (int *)&dib_item_count_, 0, KeyValueConfig::EVALUE_INT10);
    if (dib_item_count_ > 100) {
        ZEN_Trace_LogMsg::debug_errorex("load dibdata fail. invalid dibcount=%d max=%d", dib_item_count_, 100);
        return DIB_ERROR_ITEM_COUNT;
    }

    for (i = 0; i < dib_item_count_; i++) {
        snprintf(key_id, 0x3f, "dib%d.id", i + 1);
        snprintf(key_type, 0x3f, "dib%d.type", i + 1);
        snprintf(key_file_name, 0x3f, "dib%d.file", i + 1);
        snprintf(key_key_len, 0x3f, "dib%d.keylen", i + 1);

        dib_cfg.get_int(key_id, &dib_item_array_[i].id_, 0, KeyValueConfig::EVALUE_INT10);
        dib_cfg.get_int(key_type, &dib_item_array_[i].type_, 0, KeyValueConfig::EVALUE_INT10);
        dib_cfg.get_int(key_key_len, (int *)&dib_item_array_[i].key_len_, 0, KeyValueConfig::EVALUE_INT10);
        dib_cfg.get_str(key_file_name, value_file_name, 0x40, "");

        if (dib_item_array_[i].type_ == 2) {
            dib_item_array_[i].dib_data_len_ = 0x4000;
            snprintf(dib_path, sizeof(dib_path), "%s/%s", cfg_dir, value_file_name);
            ret = load_bin_file(dib_path, dib_item_array_[i].dib_data_, &dib_item_array_[i].dib_data_len_);
            if (ret != 0) {
                ZEN_Trace_LogMsg::debug_errorex("load dib file fail. ret=%d file=%s", ret, dib_path);
                return DIB_ERROR_FILE_LOAD;
            }
        }
    }

    dib_cfg.get_int("keyrange.count", (int *)&key_info_count_, 0, KeyValueConfig::EVALUE_INT10);
    if (key_info_count_ > 100) {
        ZEN_Trace_LogMsg::debug_errorex("dibdata load fail. invalid keyinfo count. count=%d max=%d", key_info_count_, 100);
        return DIB_ERROR_KEY_COUNT;
    }

    for (j = 0; j < key_info_count_; j++) {
        snprintf(v_start, 0x40, "keyrange%d.v.start", j + 1);
        snprintf(f_start, 0x40, "keyrange%d.f.start", j + 1);
        snprintf(range_len, 0x40, "keyrange%d.len", j + 1);
        dib_cfg.get_int(v_start, &key_info_array_[j].vstart_, 0, KeyValueConfig::EVALUE_INT16);
        dib_cfg.get_int(f_start, &key_info_array_[j].fstart_, 0, KeyValueConfig::EVALUE_INT16);
        dib_cfg.get_int(range_len, &key_info_array_[j].len_, 0, KeyValueConfig::EVALUE_INT16);
    }

    return 0;
}
