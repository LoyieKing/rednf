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
public:
    /* ORIG 静态数据成员（inc/des2p.h；ELF 符号 _ZN6CDes2p12FUCKING_P2_*E）。
       ORIG 把它们声明在 private 段；这里放 public 只是让 emission TU 的字节
       转储能读它们，访问权限不参与类布局；尺寸断言在
       emission/crypto_static_data.cpp。 */
    static const unsigned char FUCKING_P2_LS[16];
    static const unsigned char FUCKING_P2_PC1[64];
    static const unsigned char FUCKING_P2_PC2[48];
    static const unsigned char FUCKING_P2_IP[64];
    static const unsigned char FUCKING_P2_E[48];
    static const unsigned char FUCKING_P2_ST[8][64];
    static const unsigned char FUCKING_P2_P[32];
    static const unsigned char FUCKING_P2_FP[64];
};

#endif // SECSVR_INC_DES2P_H_H_
