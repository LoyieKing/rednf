/* md2.h -- RFC 1319 MD2 的 tencrypt_new 变体
   由 DWARF 重建：struct __md2 / typedef md2_t / class CMd2
   符号核对：_ZN4CMd28md2_initEP5__md2 等。 */

#ifndef TENCRYPT_INC_MD2_H_
#define TENCRYPT_INC_MD2_H_

#include <stddef.h>

/* MD2 context: 92 字节 = 4 + 1 + 3(pad) + 4 + 48 + 16 + 16 */
typedef struct __md2 {
    unsigned int magic;      /* 魔数校验 */
    unsigned char l;         /* 校验和低位 */
    unsigned int mlen;       /* 消息长度 */
    unsigned char x[48];     /* 算法状态 */
    unsigned char c[16];     /* 校验和 */
    unsigned char m[16];     /* 消息块 */
} md2_t;

class CMd2 {
public:
    CMd2();
    ~CMd2();
    void Init();
    void UnInit();
    int md2_init(md2_t *ctx);
    int md2_update_data(md2_t *ctx, unsigned char *data, unsigned int len);
    int md2_finish(md2_t *ctx, unsigned char *digest, unsigned int len);
private:
    void md2_append_padding_and_checksum(md2_t *ctx);
    void md2_update_digest_and_checksum(md2_t *ctx);
};

#endif /* TENCRYPT_INC_MD2_H_ */
