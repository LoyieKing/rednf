// Auto-generated header stub from DWARF info
// Original path: inc/desmo.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESMO_H_H_
#define SECSVR_INC_DESMO_H_H_

// sizeof = 8
struct CDesMo { // line 7
private:
static const unsigned char FUCKING_MOBBIT[];
static const unsigned char FUCKING_MOBB[];
static const unsigned char FUCKING_MOIP[];
void *m_pSys;
void *m_pLocal;
public:
CDesMo(); // line 9
~CDesMo(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Des_Mo(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void mo_crypt(bool arg0, unsigned char *arg1, unsigned char *arg2); // line 17
};
extern const unsigned char FUCKING_MOBBIT[]; // line 19
extern const unsigned char FUCKING_MOBB[]; // line 20
extern const unsigned char FUCKING_MOIP[]; // line 21

#endif // SECSVR_INC_DESMO_H_H_
