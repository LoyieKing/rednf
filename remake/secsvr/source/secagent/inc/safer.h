// Auto-generated header stub from DWARF info
// Original path: inc/safer.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SAFER_H_H_
#define SECSVR_INC_SAFER_H_H_

// sizeof = 24
struct CSafer { // line 7
private:
unsigned char *l_key;
unsigned char *logf;
unsigned char *expf;
unsigned int k_bytes;
void *m_pSys;
void *m_pLocal;
public:
CSafer(); // line 9
~CSafer(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Safer(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetKey(unsigned char *arg0, unsigned int arg1); // line 17
void do_fr(unsigned char *arg0, unsigned char *arg1); // line 18
void do_ir(unsigned char *arg0, unsigned char *arg1); // line 19
void encrypt(unsigned char *arg0); // line 20
void decrypt(unsigned char *arg0); // line 21
};

#endif // SECSVR_INC_SAFER_H_H_
