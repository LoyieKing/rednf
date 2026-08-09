// Auto-generated header stub from DWARF info
// Original path: inc/gramary.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_GRAMARY_H_H_
#define SECSVR_INC_GRAMARY_H_H_

// sizeof = 8
struct CGramary { // line 7
private:
static const unsigned char FUCKING_GRABB[];
static const unsigned char FUCKING_GRAIP[];
void *m_pSys;
void *m_pLocal;
public:
CGramary(); // line 9
~CGramary(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Gramary(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void gra_crypt(bool arg0, unsigned char *arg1, unsigned char *arg2); // line 17
unsigned char gra_round(unsigned char arg0, unsigned char arg1, unsigned char arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5, unsigned char arg6, unsigned char arg7, unsigned char arg8); // line 18
};
extern const unsigned char FUCKING_GRABB[]; // line 21
extern const unsigned char FUCKING_GRAIP[]; // line 22

#endif // SECSVR_INC_GRAMARY_H_H_
