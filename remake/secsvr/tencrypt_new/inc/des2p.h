// Auto-generated header stub from DWARF info
// Original path: inc/des2p.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DES2P_H_H_
#define SECSVR_INC_DES2P_H_H_

// sizeof = 768
struct des_key_schedule { // line 7
public:
unsigned char b[16][48];
};
typedef des_key_schedule des_key_schedule;
// sizeof = 8
struct CDes2p { // line 12
private:
void *m_pSys;
void *m_pLocal;
public:
CDes2p(); // line 14
~CDes2p(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void Des2p(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
private:
void des_set_odd_parity(unsigned char *arg0); // line 23
int des_ecb_encrypt(unsigned char *arg0, unsigned char *arg1, des_key_schedule *arg2, bool arg3); // line 24
void des_func(unsigned char *arg0, unsigned char *arg1, unsigned char (*arg2)[48], bool arg3); // line 25
int des_set_key(unsigned char *arg0, des_key_schedule *arg1); // line 26
void unbit(unsigned char *arg0, unsigned char *arg1, int arg2); // line 27
void subs(unsigned char *arg0, unsigned char *arg1, int arg2); // line 28
void add2(unsigned char *arg0, unsigned char *arg1, int arg2); // line 29
void rot(unsigned char *arg0, int arg1, int arg2); // line 30
void perm(unsigned char *arg0, const unsigned char *arg1, unsigned char *arg2, int arg3); // line 31
void pkbit(unsigned char *arg0, unsigned char *arg1, int arg2); // line 32
};

#endif // SECSVR_INC_DES2P_H_H_
