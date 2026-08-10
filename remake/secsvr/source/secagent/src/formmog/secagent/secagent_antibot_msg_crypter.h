// Reconstructed from secagent disassembly
// Original path: src/formmog/secagent/secagent_antibot_msg_crypter.h
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_

#include "src/protocol/formmog/formmog_proto_antibot_client.h"

// sizeof = 1
struct AntibotCrypter { // line 25
private:
    static const unsigned int MAX_CHECK_DATA_LEN;
public:
    AntibotCrypter(); // line 27
    ~AntibotCrypter(); // line 28
    static int encrypt(unsigned int func_id, const unsigned char *key, unsigned char *data,
                       unsigned int data_len, sec_proto::AntibotClientPkg *msg); // line 35
    static int decrypt(unsigned int func_id, const unsigned char *key,
                       sec_proto::AntibotClientPkg *msg); // line 41
    static int encrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf,
                       unsigned int buf_len); // line 45
    static int decrypt(unsigned int func_id, const unsigned char *key, unsigned char *buf,
                       unsigned int buf_len); // line 47
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_ANTIBOT_MSG_CRYPTER_H_H_
