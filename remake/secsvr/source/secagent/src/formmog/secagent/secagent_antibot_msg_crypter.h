// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_antibot_msg_crypter.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_

// sizeof = 1
struct AntibotCrypter { // line 25
private:
static const unsigned int MAX_CHECK_DATA_LEN;
public:
AntibotCrypter(); // line 27
~AntibotCrypter(); // line 28
static int encrypt(unsigned int func_id, const unsigned char *key, unsigned char *data, unsigned int data_len, /*anon struct*/ int *msg); // line 35
static int decrypt(unsigned int func_id, const unsigned char *key, /*anon struct*/ int *msg); // line 41
static int encrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf, unsigned int buf_len); // line 45
static int decrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf, unsigned int buf_len); // line 47
};


extern bool is_valid_func_id(const unsigned int func_id); // line 43 (inferred)
extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_
