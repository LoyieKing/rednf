// Auto-generated header stub from DWARF info
// Original path: inc/md2.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MD2_H_H_
#define SECSVR_INC_MD2_H_H_

// sizeof = 92
struct __md2 { // line 15
public:
unsigned int magic;
unsigned char l;
unsigned int mlen;
unsigned char x[];
unsigned char c[];
unsigned char m[];
};
typedef __md2 md2_t;
// sizeof = 8
struct CMd2 { // line 25
private:
unsigned char *padding[];
unsigned char *S;
public:
CMd2(); // line 28
~CMd2(); // line 29
void Init(); // line 31
void UnInit(); // line 32
int md2_init(md2_t *arg0); // line 34
int md2_update_data(md2_t *arg0, unsigned char *arg1, unsigned int arg2); // line 36
int md2_finish(md2_t *arg0, unsigned char *arg1, unsigned int arg2); // line 38
private:
void md2_append_padding_and_checksum(md2_t *arg0); // line 41
void md2_update_digest_and_checksum(md2_t *arg0); // line 43
};

#endif // SECSVR_INC_MD2_H_H_
