// Auto-generated header stub from DWARF info
// Original path: inc/feal8.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_FEAL8_H_H_
#define SECSVR_INC_FEAL8_H_H_

// sizeof = 88
struct CFeal8 { // line 7
private:
static const unsigned char FUCKING_ROT2VAL[];
unsigned int K[];
unsigned int K89;
unsigned int K1011;
unsigned int K1213;
unsigned int K1415;
void *m_pSys;
void *m_pLocal;
public:
CFeal8(); // line 9
~CFeal8(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Feal8(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void Decrypt(unsigned char *arg0, unsigned char *arg1); // line 17
void Encrypt(unsigned char *arg0, unsigned char *arg1); // line 18
void SetKey(unsigned char *arg0); // line 19
void DissH1(unsigned int arg0, unsigned char *arg1); // line 20
unsigned int MakeH2(unsigned int *arg0); // line 21
void DissQ1(unsigned int arg0, unsigned char *arg1); // line 22
unsigned int FK(unsigned int arg0, unsigned int arg1); // line 23
unsigned int f(unsigned int arg0, unsigned int arg1); // line 24
unsigned char Rot2(unsigned char arg0); // line 25
void InitRot2Value(); // line 26
unsigned int MakeH1(unsigned char *arg0); // line 27
};
extern const unsigned char FUCKING_ROT2VAL[]; // line 30

#endif // SECSVR_INC_FEAL8_H_H_
