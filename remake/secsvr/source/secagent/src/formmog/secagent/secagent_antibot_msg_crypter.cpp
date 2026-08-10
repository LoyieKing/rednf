// Reconstructed from secagent disassembly (encrypt/decrypt dispatch table
// `encrypt_funcs` 逐项对照二进制 .data.rel.ro）。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_antibot_msg_crypter.cpp

#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/protocol/formmog/formmog_proto_antibot_client.h"
#include "import/include/tencore/tencrypt/TenCrypt.h"
#include "src/commlib/zenlib/zen_crc32.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/framework/comm_stat_define.h"

#include <string.h>

#define ANTIBOT_ERROR_INVALID_FUNC_ID 0x493f9
#define ANTIBOT_ERROR_CRC32_FAIL      0x493fa

static bool is_valid_func_id(const unsigned int func_id) {
    return func_id - 1 <= 0x19;
}

// encrypt_funcs 表逐项对照二进制 .data.rel.ro 0x081fd780（27 项，[0]/[26] 边界
// 与符号表核对；[26] = TenTean）
static void (*const encrypt_funcs[])(unsigned char *, bool, unsigned char *, unsigned int) = {
    NULL,
    TenCast, TenDesBig, TenBlowFish8, TenBlowFish16, TenBlowFish24, TenBlowFish32,
    TenBlowFish56, TenMars, TenAdder, TenD3des161, TenD3des162, TenDes2P, TenDesMo,
    TenDesNew, TenDesPC, TenDesToo, TenFastDes, TenFeal8, TenGene, TenGost,
    TenGramary, TenIdea, TenRc6, TenShift, TenSubtracter, TenTean
};

AntibotCrypter::AntibotCrypter() {
}

AntibotCrypter::~AntibotCrypter() {
}

int AntibotCrypter::decrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf, unsigned int buf_len) {
    if (!is_valid_func_id(func_id)) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], Bad decrypt func id|func_id=%d", __PRETTY_FUNCTION__, func_id);
        return ANTIBOT_ERROR_INVALID_FUNC_ID;
    }
    encrypt_funcs[func_id]((unsigned char *)key, false, buf, buf_len);
    return 0;
}

int AntibotCrypter::encrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf, unsigned int buf_len) {
    if (!is_valid_func_id(func_id)) {
        ZEN_Trace_LogMsg::debug_errorex("Bad encrypt func id|func_id=%d", func_id);
        return ANTIBOT_ERROR_INVALID_FUNC_ID;
    }
    encrypt_funcs[func_id]((unsigned char *)key, true, buf, buf_len);
    return 0;
}

int AntibotCrypter::decrypt(unsigned int func_id, const unsigned char *key, sec_proto::AntibotClientPkg *msg) {
    if (!is_valid_func_id(func_id)) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], invalid decrypt func id|decrypt_func_id=%d", __PRETTY_FUNCTION__, func_id);
        return ANTIBOT_ERROR_INVALID_FUNC_ID;
    }
    unsigned int crc32_value;
    // 线上包字节布局：+0x00 u16 func_id；+0x06 u32 crc32；+0x0a u16 data_len；+0x0c data[]
    unsigned char *pkg = (unsigned char *)msg;
    unsigned int data_len = *(unsigned short *)(pkg + 0xa);
    int ret = decrypt(func_id, key, pkg + 0xc, data_len);
    crc32_value = Crc32(pkg + 0xc, data_len);
    if (crc32_value != *(unsigned int *)(pkg + 0x6)) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], decrypt crc32 verify fail|decrypt_func_id=%d", __PRETTY_FUNCTION__, func_id);
        return ANTIBOT_ERROR_CRC32_FAIL;
    }
    ZEN_Trace_LogMsg::debug_infoex("antibot pkg decrypt succ");
    return 0;
}

int AntibotCrypter::encrypt(unsigned int func_id, const unsigned char *key, unsigned char *data, unsigned int data_len, sec_proto::AntibotClientPkg *msg) {
    if (!is_valid_func_id(func_id)) {
        ZEN_Trace_LogMsg::debug_errorex("Invalid encrypt func id|encrypt_func_id=%d", func_id);
        return ANTIBOT_ERROR_INVALID_FUNC_ID;
    }
    int ret;
    unsigned char *pkg = (unsigned char *)msg;
    *(unsigned int *)(pkg + 0x6) = Crc32(data, data_len);
    ret = encrypt(func_id, key, data, data_len);
    *(unsigned short *)(pkg + 0x0) = 0x25;
    *(unsigned short *)(pkg + 0xa) = (unsigned short)data_len;
    memcpy(pkg + 0xc, data, data_len);
    return ret;
}
