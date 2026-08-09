// Auto-generated header stub from DWARF info
// Original path: inc/despc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESPC_H_H_
#define SECSVR_INC_DESPC_H_H_

// sizeof = 24792
struct CDesPC { // line 7
private:
unsigned char s18[4][4096];
unsigned char pc_iperm[16][16][8];
unsigned char pc_fperm[16][16][8];
unsigned char pc_pc1m[56];
unsigned char pc_pcr[56];
unsigned char pc_kn[16][6];
unsigned char pc_p32[4][256][4];
void *m_pSys;
void *m_pLocal;
public:
CDesPC(); // line 9
~CDesPC(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void DesPC(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void dedes(unsigned char *arg0, unsigned char *arg1); // line 26
void endes(unsigned char *arg0, unsigned char *arg1); // line 27
void iter(int arg0, unsigned char *arg1, unsigned char *arg2); // line 28
void pc_f(unsigned char *arg0, int arg1, unsigned char *arg2); // line 29
void perm32(unsigned char *arg0, unsigned char *arg1); // line 30
void contract(unsigned char *arg0, unsigned char *arg1); // line 31
void expand(unsigned char *arg0, unsigned char *arg1); // line 32
void pc_permute(unsigned char *arg0, unsigned char (*arg1)[16][8], unsigned char *arg2); // line 33
unsigned char getcomp(unsigned int arg0, unsigned int arg1); // line 37
void kinit(unsigned char *arg0); // line 38
void pc_perminit(unsigned char (*arg0)[16][8], const unsigned char *arg1); // line 39
};

#endif // SECSVR_INC_DESPC_H_H_
