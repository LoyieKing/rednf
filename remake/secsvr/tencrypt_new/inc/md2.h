/* md2.h -- RFC 1319 MD2 的 tencrypt_new 变体
   类布局来自 ORIG DWARF（inc/md2.h:25，sizeof = 8）：
     padding : +0  unsigned char (*)[16]  16x16 填充表（第 n-1 行 = n 个 n）
     S       : +4  unsigned char *         PI 置换表（256 B；ORIG .rodata
                                           CMd2::Init()::C.15 @0x259ac0）
   符号核对：_ZN4CMd24InitEv / _ZN4CMd26UnInitEv / _ZN4CMd28md2_initEP5__md2
   等，全部在 libtss_sdk.so.1.0.394 中存在且签名一致。 */

#ifndef TENCRYPT_INC_MD2_H_
#define TENCRYPT_INC_MD2_H_

#include <stddef.h>

/* MD2 context: 92 字节 = 4 + 1 + 3(pad) + 4 + 48 + 16 + 16
   ORIG 偏移：magic +0x00, l +0x04, mlen +0x08, x +0x0c, c +0x3c, m +0x4c */
typedef struct __md2 {
    unsigned int magic;      /* md2_init 写入 0xf00fd00d，其余入口校验 */
    unsigned char l;         /* 校验和运行值（跨块保持） */
    unsigned int mlen;       /* ctx->m 内已缓冲字节数 0..15（不是总长度） */
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
    unsigned char (*padding)[16];   /* ORIG +0x00 */
    unsigned char *S;               /* ORIG +0x04 */
    void md2_append_padding_and_checksum(md2_t *ctx);
    void md2_update_digest_and_checksum(md2_t *ctx);
};

#endif /* TENCRYPT_INC_MD2_H_ */
