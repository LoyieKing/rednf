// Auto-generated header stub from DWARF info
// Original path: inc/rc2.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RC2_H_H_
#define SECSVR_INC_RC2_H_H_

// sizeof = 16
struct CRc2 { // line 7
private:
unsigned char *permute;
short unsigned int *xkey;
void *m_pSys;
void *m_pLocal;
public:
CRc2(); // line 9
~CRc2(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Rc2(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void decrypt(unsigned char *arg0, unsigned char *arg1); // line 17
void encrypt(unsigned char *arg0, unsigned char *arg1); // line 18
bool set_key(unsigned char *arg0, unsigned int arg1); // line 19
};

#endif // SECSVR_INC_RC2_H_H_
