// Auto-generated header stub from DWARF info
// Original path: inc/desbig.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESBIG_H_H_
#define SECSVR_INC_DESBIG_H_H_

// sizeof = 8
struct CDesBig { // line 7
private:
static const unsigned int FUCKING_SX_TAB[][];
static const unsigned char FUCKING_KS_TAB[];
static const unsigned int FUCKING_P2_TAB[][];
void *m_pSys;
void *m_pLocal;
public:
CDesBig(); // line 9
~CDesBig(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void DesBig(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void des_ky(void *arg0, void *arg1); // line 17
void des_ecm(void *arg0, void *arg1, void *arg2); // line 18
void des_dc(void *arg0, void *arg1, void *arg2); // line 19
void des_ec(void *arg0, void *arg1, void *arg2); // line 20
};
extern const unsigned int FUCKING_SX_TAB[][]; // line 24
extern const unsigned char FUCKING_KS_TAB[]; // line 26
extern const unsigned int FUCKING_P2_TAB[][]; // line 28

#endif // SECSVR_INC_DESBIG_H_H_
