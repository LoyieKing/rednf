// Reconstructed from secagent disassembly
// Original path: src/formmog/secagent/secagent_antibot_crypt_data.h
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_

#include "src/formmog/secagent/secagent_static_config_dec.h"

// sizeof = 16400
struct DibItem { // line 16
public:
    int id_;
    int type_;
    unsigned int dib_data_len_;
    unsigned char dib_data_[MAX_DIB_LEN]; // 16384
    unsigned int key_len_;
};

// sizeof = 12
struct DibKeyInfo { // line 28
public:
    int vstart_;
    int fstart_;
    int len_;
};

// sizeof = 1641224
struct DibData { // line 36
public:
    unsigned char ver_[16];
    unsigned int dib_item_count_;
    DibItem dib_item_array_[MAX_DIB_ITEM_COUNT];
    unsigned int key_info_count_;
    DibKeyInfo key_info_array_[MAX_KEY_INFO_COUNT];
    DibData(); // line 38
    ~DibData(); // line 39
    int load(const char *cfg_dir); // line 42
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_
