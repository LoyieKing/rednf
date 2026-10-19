// Auto-generated header stub from DWARF info
// Original path: inc/3way.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_3WAY_H_H_
#define SECSVR_INC_3WAY_H_H_

// sizeof = 8
struct CThreeWay { // line 7
private:
void *m_pSys;
void *m_pLocal;
public:
CThreeWay(); // line 9
~CThreeWay(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void ThreeWay(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void Encrypt(unsigned int *arg0, unsigned int *arg1); // line 19
void Decrypt(unsigned int *arg0, unsigned int *arg1); // line 20
void mu(unsigned int *arg0); // line 21
void gamma(unsigned int *arg0); // line 22
void theta(unsigned int *arg0); // line 23
void pi_1(unsigned int *arg0); // line 24
void pi_2(unsigned int *arg0); // line 25
void rho(unsigned int *arg0); // line 26
void rndcon_gen(unsigned int arg0, unsigned int *arg1); // line 27
};

#endif // SECSVR_INC_3WAY_H_H_
