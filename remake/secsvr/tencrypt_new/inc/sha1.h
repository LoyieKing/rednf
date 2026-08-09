/* sha1.h -- Steve Reid 公版 SHA-1（RFC 3174）的 tencrypt_new 变体
   由 DWARF 重建：struct _SHA1_CTX / typedef SHA1_CTX / class CSha1
   符号核对：_ZN5CSha110SHA1_InitEP10_SHA1_CTX、_ZN5CSha110SHA1_FinalEPhP10_SHA1_CTX 等。 */

#ifndef TENCRYPT_INC_SHA1_H_
#define TENCRYPT_INC_SHA1_H_

#include <stddef.h>

/* SHA-1 context: 92 字节（DWARF sizeof 验证） */
typedef struct _SHA1_CTX {
    unsigned int state[5];  /* 20 字节 */
    unsigned int count[2];  /* 8 字节 */
    unsigned char buffer[64]; /* 64 字节 */
} SHA1_CTX;

class CSha1 {
public:
    CSha1();
    ~CSha1();
    void SHA1_Init(SHA1_CTX *context);
    void SHA1_Update(SHA1_CTX *context, unsigned char *data, unsigned int len);
    void SHA1_Final(unsigned char *digest, SHA1_CTX *context);
private:
    void SHA1Transform(unsigned int *state, unsigned char *buffer);
};

#endif /* TENCRYPT_INC_SHA1_H_ */
