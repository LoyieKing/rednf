/* md5.h -- L. Peter Deutsch 公版 MD5（RFC 1321 参考实现头）
   由 DWARF 重建：struct md5_state_s / typedef md5_state_t / md5_init|append|finish|process
   符号核对：_Z11md5_processP11md5_state_sPKh 等。 */

#ifndef TENCRYPT_INC_MD5_H_
#define TENCRYPT_INC_MD5_H_

#include <stddef.h>

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t;  /* 32-bit word */

/* MD5 context: 88 字节（DWARF sizeof 验证） */
typedef struct md5_state_s {
    md5_word_t count[2];   /* 消息长度（位），8 字节 */
    md5_word_t abcd[4];    /* 摘要缓冲区，16 字节 */
    md5_byte_t buf[64];    /* 未处理的数据块，64 字节 */
} md5_state_t;

#ifdef __cplusplus
extern "C" {
#endif
void md5_init(md5_state_t *pms);
void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
void md5_finish(md5_state_t *pms, md5_byte_t *digest);
#ifdef __cplusplus
}
#endif

/* 内部压缩函数（C++ 链接，mangled: _Z11md5_processP11md5_state_sPKh；
   二进制符号表绑定为 t 是 strip/localize 的结果，mangled 名为全局形式） */
void md5_process(md5_state_t *pms, const md5_byte_t *data);

#endif /* TENCRYPT_INC_MD5_H_ */
