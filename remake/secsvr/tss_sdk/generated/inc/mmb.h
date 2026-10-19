// Auto-generated header stub from DWARF info
// Original path: inc/mmb.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MMB_H_H_
#define SECSVR_INC_MMB_H_H_

// sizeof = 16
struct CMmb { // line 7
private:
unsigned int *key;
unsigned int *iv_tbl;
void *m_pSys;
void *m_pLocal;
public:
CMmb(); // line 9
~CMmb(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Mmb(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetKey(unsigned int *arg0); // line 17
unsigned int ModMult(unsigned int arg0, unsigned int arg1); // line 18
void f(unsigned int *arg0); // line 19
void g(unsigned int *arg0); // line 20
void Encrypt(unsigned int *arg0); // line 21
void Decrypt(unsigned int *arg0); // line 22
};

#endif // SECSVR_INC_MMB_H_H_
