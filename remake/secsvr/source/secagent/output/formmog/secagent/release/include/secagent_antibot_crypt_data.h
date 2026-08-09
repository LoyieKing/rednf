// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_antibot_crypt_data.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_

// sizeof = 16400
struct DibItem { // line 16
public:
int id_;
int type_;
unsigned int dib_data_len_;
unsigned char dib_data_[];
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
unsigned char ver_[];
unsigned int dib_item_count_;
DibItem dib_item_array_[];
unsigned int key_info_count_;
DibKeyInfo key_info_array_[];
DibData(); // line 38
~DibData(); // line 39
int load(const char *cfg_dir); // line 42
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_CRYPT_DATA_H_H_
