// Auto-generated header stub from DWARF info
// Original path: inc/md5.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MD5_H_H_
#define SECSVR_INC_MD5_H_H_

typedef unsigned char md5_byte_t;
typedef unsigned int md5_word_t;
// sizeof = 88
struct md5_state_s { // line 59
public:
md5_word_t count[];
md5_word_t abcd[];
md5_byte_t buf[];
};
typedef md5_state_s md5_state_t;


extern void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes); // line 286 (inferred)
extern void md5_finish(md5_state_t *pms, md5_byte_t *digest); // line 324 (inferred)
extern void md5_init(md5_state_t *pms); // line 276 (inferred)
extern void md5_process(md5_state_t *pms, const md5_byte_t *data); // line 118 (inferred)

#endif // SECSVR_INC_MD5_H_H_
