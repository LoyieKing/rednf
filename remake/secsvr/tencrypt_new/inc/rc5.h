// Auto-generated header stub from DWARF info
// Original path: inc/rc5.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RC5_H_H_
#define SECSVR_INC_RC5_H_H_

// sizeof = 12
struct CRc5 { // line 7
private:
unsigned int *S;
void *m_pSys;
void *m_pLocal;
public:
CRc5(); // line 9
~CRc5(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Rc5(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void decrypt(unsigned int *arg0, unsigned int *arg1); // line 17
void encrypt(unsigned int *arg0, unsigned int *arg1); // line 18
void set_key(unsigned char *arg0); // line 19
};

#endif // SECSVR_INC_RC5_H_H_
