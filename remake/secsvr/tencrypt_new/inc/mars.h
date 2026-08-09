// Auto-generated header stub from DWARF info
// Original path: inc/mars.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MARS_H_H_
#define SECSVR_INC_MARS_H_H_

// sizeof = 228
struct CMars { // line 11
private:
static const unsigned int FUCKING_S_BOX[];
static const unsigned int FUCKING_IM1[];
static const unsigned int FUCKING_IM2[];
static const unsigned int FUCKING_IM7[];
static const unsigned int FUCKING_I4M[];
static const unsigned int FUCKING_B_TAB[];
unsigned int ll_key[];
unsigned int t_key[];
void *m_pSys;
void *m_pLocal;
public:
CMars(); // line 13
~CMars(); // line 14
void Init(void *arg0, void *arg1); // line 16
void UnInit(); // line 17
void Mars(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 19
private:
void mars_dec(unsigned char *arg0, unsigned char *arg1); // line 21
void mars_enc(unsigned char *arg0, unsigned char *arg1); // line 22
void mars_setkey(unsigned char *arg0, unsigned int arg1); // line 23
};
extern const unsigned int FUCKING_S_BOX[]; // line 26
extern const unsigned int FUCKING_IM1[]; // line 27
extern const unsigned int FUCKING_IM2[]; // line 28
extern const unsigned int FUCKING_IM7[]; // line 29
extern const unsigned int FUCKING_I4M[]; // line 30
extern const unsigned int FUCKING_B_TAB[]; // line 31

#endif // SECSVR_INC_MARS_H_H_
