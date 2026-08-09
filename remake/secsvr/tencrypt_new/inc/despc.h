// Auto-generated header stub from DWARF info
// Original path: inc/despc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESPC_H_H_
#define SECSVR_INC_DESPC_H_H_

// sizeof = 24792
struct CDesPC { // line 7
private:
static const unsigned int FUCKING_PC_NIBB[];
static const unsigned int FUCKING_PC_BBIT[];
static const unsigned char FUCKING_PC_IP[];
static const unsigned char FUCKING_PC_FP[];
static const unsigned char FUCKING_PC_PC1[];
static const unsigned char FUCKING_PC_PC2[];
static const unsigned char FUCKING_PC_P32I[];
static const unsigned char FUCKING_PC_SI[][];
static const unsigned char FUCKING_PC_TTT[];
unsigned char s18[][];
unsigned char pc_iperm[][][];
unsigned char pc_fperm[][][];
unsigned char pc_pc1m[];
unsigned char pc_pcr[];
unsigned char pc_kn[][];
unsigned char pc_p32[][][];
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
void pc_permute(unsigned char *arg0, unsigned char *arg1[][], unsigned char *arg2); // line 33
void pc_desinit(unsigned char *arg0); // line 34
void p32init(); // line 35
void sinit(); // line 36
unsigned char getcomp(unsigned int arg0, unsigned int arg1); // line 37
void kinit(unsigned char *arg0); // line 38
void pc_perminit(unsigned char *arg0[][], const unsigned char *arg1); // line 39
};
extern const unsigned int FUCKING_PC_NIBB[]; // line 43
extern const unsigned int FUCKING_PC_BBIT[]; // line 44
extern const unsigned char FUCKING_PC_IP[]; // line 45
extern const unsigned char FUCKING_PC_FP[]; // line 46
extern const unsigned char FUCKING_PC_PC1[]; // line 47
extern const unsigned char FUCKING_PC_PC2[]; // line 48
extern const unsigned char FUCKING_PC_P32I[]; // line 49
extern const unsigned char FUCKING_PC_SI[][]; // line 51
extern const unsigned char FUCKING_PC_TTT[]; // line 53

#endif // SECSVR_INC_DESPC_H_H_
