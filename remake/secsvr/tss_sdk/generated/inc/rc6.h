// Auto-generated header stub from DWARF info
// Original path: inc/rc6.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RC6_H_H_
#define SECSVR_INC_RC6_H_H_

// sizeof = 184
struct CRc6 { // line 8
private:
unsigned int l_key[];
void *m_pSys;
void *m_pLocal;
public:
CRc6(); // line 10
~CRc6(); // line 11
void Init(void *arg0, void *arg1); // line 13
void UnInit(); // line 14
void Rc6(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 16
private:
void decrypt(unsigned int *arg0, unsigned int *arg1); // line 18
void encrypt(unsigned int *arg0, unsigned int *arg1); // line 19
unsigned int * set_key(unsigned char *arg0); // line 20
};

#endif // SECSVR_INC_RC6_H_H_
