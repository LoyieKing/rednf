// Auto-generated header stub from DWARF info
// Original path: inc/gosthash.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_GOSTHASH_H_H_
#define SECSVR_INC_GOSTHASH_H_H_

// sizeof = 132
struct ._30 { // line 7
public:
unsigned int sum[];
unsigned int hash[];
unsigned int len[];
unsigned char partial[];
unsigned int partial_bytes;
};
// sizeof = 20
struct CGostHash { // line 16
private:
unsigned int *m_psbox_1;
unsigned int *m_psbox_2;
unsigned int *m_psbox_3;
unsigned int *m_psbox_4;
._30 *m_pctx;
public:
CGostHash(); // line 18
~CGostHash(); // line 19
void InitBoxData(); // line 21
void gosthash_reset(); // line 23
void gosthash_update(unsigned char *arg0, unsigned int arg1); // line 24
void gosthash_final(unsigned char *arg0); // line 25
private:
void gosthash_compress(unsigned int *arg0, unsigned int *arg1); // line 28
void gosthash_bytes(unsigned char *arg0, unsigned int arg1); // line 29
};
// sizeof = 132
struct ._0 { // line 7
public:
unsigned int sum[];
unsigned int hash[];
unsigned int len[];
unsigned char partial[];
unsigned int partial_bytes;
};

#endif // SECSVR_INC_GOSTHASH_H_H_
