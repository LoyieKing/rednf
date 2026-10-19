// Auto-generated header stub from DWARF info
// Original path: inc/destoo.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESTOO_H_H_
#define SECSVR_INC_DESTOO_H_H_

// sizeof = 6280
struct CDesToo { // line 7
private:
unsigned char too_iperm[][][];
unsigned char too_fperm[][][];
unsigned int too_sp[][];
unsigned char too_kn[][];
static const unsigned char FUCKING_TOO_PC1[];
static const unsigned int FUCKING_TOO_BYTEBIT[];
static const unsigned char FUCKING_TOO_TOTROT[];
static const unsigned char FUCKING_TOO_PC2[];
static const unsigned char FUCKING_TOO_IP[];
static const unsigned char FUCKING_TOO_FP[];
static const unsigned int FUCKING_TOO_NIBBLEBIT[];
static const unsigned char FUCKING_TOO_P32I[];
static const unsigned char FUCKING_TOO_SI[][];
static const unsigned int FUCKING_TOO_SP[][];
void *m_pSys;
void *m_pLocal;
public:
CDesToo(); // line 9
~CDesToo(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void DesToo(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void too_setkey(unsigned char *arg0); // line 17
void spinit(); // line 18
void too_perminit(unsigned char *arg0[][], const unsigned char *arg1); // line 19
void dodecrypt(unsigned char *arg0); // line 20
void doencrypt(unsigned char *arg0); // line 21
void round(int arg0, unsigned int *arg1); // line 22
void too_permute(unsigned char *arg0, unsigned char *arg1[][], unsigned char *arg2); // line 23
unsigned int fsf(unsigned int arg0, unsigned char *arg1); // line 24
};
extern const unsigned char FUCKING_TOO_PC1[]; // line 40
extern const unsigned int FUCKING_TOO_BYTEBIT[]; // line 41
extern const unsigned char FUCKING_TOO_TOTROT[]; // line 42
extern const unsigned char FUCKING_TOO_PC2[]; // line 43
extern const unsigned char FUCKING_TOO_IP[]; // line 44
extern const unsigned char FUCKING_TOO_FP[]; // line 45
extern const unsigned int FUCKING_TOO_NIBBLEBIT[]; // line 46
extern const unsigned char FUCKING_TOO_P32I[]; // line 47
extern const unsigned char FUCKING_TOO_SI[][]; // line 48
extern const unsigned int FUCKING_TOO_SP[][]; // line 49

#endif // SECSVR_INC_DESTOO_H_H_
