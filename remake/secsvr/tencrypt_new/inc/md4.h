/* md4.h -- RFC 1320 MD4 的 tencrypt_new 变体
   由 DWARF + mangled 名重建：MD4_CTX / class CMD4
   符号核对：_ZN4CMD47MD4InitEP7MD4_CTX（参数类型名 MD4_CTX，7 字符）。 */

#ifndef TENCRYPT_INC_MD4_H_
#define TENCRYPT_INC_MD4_H_

#include <stddef.h>

/* MD4 context: 88 字节 = 16 + 8 + 64 */
typedef struct MD4_CTX {
    unsigned int state[4];   /* 16 字节 */
    unsigned int count[2];   /* 8 字节 */
    unsigned char buffer[64];/* 64 字节 */
} MD4_CTX;

class CMD4 {
public:
    CMD4();
    ~CMD4();
    void Init();
    void UnInit();
    void MD4Init(MD4_CTX *context);
    void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int inputLen);
    void MD4Final(unsigned char *digest, MD4_CTX *context);
private:
    void MD4Transform(unsigned int *state, unsigned char *block);
    void Encode(unsigned char *output, unsigned int *input, unsigned int len);
    void Decode(unsigned int *output, unsigned char *input, unsigned int len);
};

#endif /* TENCRYPT_INC_MD4_H_ */
