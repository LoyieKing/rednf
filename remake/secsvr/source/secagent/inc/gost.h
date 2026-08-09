// Auto-generated header stub from DWARF info
// Original path: inc/gost.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_GOST_H_H_
#define SECSVR_INC_GOST_H_H_

// sizeof = 8
struct CGost { // line 7
private:
static const unsigned int FUCKING_WZ_SPKEY[];
static const unsigned char FUCKING_WZ_SP[][];
void *m_pSys;
void *m_pLocal;
public:
CGost(); // line 9
~CGost(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Gost(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void gost_dec(unsigned int *arg0, unsigned int *arg1); // line 18
void dencry_data(unsigned int *arg0, unsigned int *arg1, unsigned int *arg2); // line 19
void gost_enc(unsigned int *arg0, unsigned int *arg1); // line 20
void encry_data(unsigned int *arg0, unsigned int *arg1, unsigned int *arg2); // line 21
void gost_swap(unsigned int *arg0, unsigned int *arg1); // line 22
unsigned int f(unsigned int arg0); // line 23
};
extern const unsigned int FUCKING_WZ_SPKEY[]; // line 26
extern const unsigned char FUCKING_WZ_SP[][]; // line 27

#endif // SECSVR_INC_GOST_H_H_
