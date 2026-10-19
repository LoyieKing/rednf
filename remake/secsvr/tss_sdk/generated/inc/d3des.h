// Auto-generated header stub from DWARF info
// Original path: inc/d3des.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_D3DES_H_H_
#define SECSVR_INC_D3DES_H_H_

// sizeof = 392
struct CD3des { // line 12
private:
static const unsigned int FUCKING_SP1[];
static const unsigned int FUCKING_SP2[];
static const unsigned int FUCKING_SP3[];
static const unsigned int FUCKING_SP4[];
static const unsigned int FUCKING_SP5[];
static const unsigned int FUCKING_SP6[];
static const unsigned int FUCKING_SP7[];
static const unsigned int FUCKING_SP8[];
static const unsigned char FUCKING_TOTROT[];
static const short unsigned int FUCKING_BYTEBIT[];
static const unsigned int FUCKING_BIGBYTE[];
static const unsigned char FUCKING_PC1[];
static const unsigned char FUCKING_PC2[];
unsigned int KnL[];
unsigned int KnR[];
unsigned int Kn3[];
static const unsigned char FUCKING_DF_KEY[];
void *m_pSys;
void *m_pLocal;
public:
CD3des(); // line 14
~CD3des(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void D3des161(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 21
void D3des162(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 25
void D3des24(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 29
private:
void make3key(char *arg0, unsigned char *arg1); // line 33
void D3des(unsigned char *arg0, unsigned char *arg1); // line 34
void use3key(unsigned int *arg0); // line 35
void cp3key(unsigned int *arg0); // line 36
void des3key(const unsigned char *arg0, short int arg1); // line 37
void make2key(char *arg0, unsigned char *arg1); // line 38
void makekey(char *arg0, unsigned char *arg1); // line 39
void D2des(unsigned char *arg0, unsigned char *arg1); // line 40
void Ddes(unsigned char *arg0, unsigned char *arg1); // line 41
void des2key(const unsigned char *arg0, short int arg1); // line 42
void desfunc(unsigned int *arg0, unsigned int *arg1); // line 43
void unscrun(unsigned int *arg0, unsigned char *arg1); // line 44
void scrunch(unsigned char *arg0, unsigned int *arg1); // line 45
void des(unsigned char *arg0, unsigned char *arg1); // line 46
void usekey(unsigned int *arg0); // line 47
void cpkey(unsigned int *arg0); // line 48
void deskey(const unsigned char *arg0, short int arg1); // line 49
};
extern const unsigned int FUCKING_SP1[]; // line 51
extern const unsigned int FUCKING_SP2[]; // line 52
extern const unsigned int FUCKING_SP3[]; // line 53
extern const unsigned int FUCKING_SP4[]; // line 54
extern const unsigned int FUCKING_SP5[]; // line 55
extern const unsigned int FUCKING_SP6[]; // line 56
extern const unsigned int FUCKING_SP7[]; // line 57
extern const unsigned int FUCKING_SP8[]; // line 58
extern const unsigned char FUCKING_TOTROT[]; // line 61
extern const short unsigned int FUCKING_BYTEBIT[]; // line 62
extern const unsigned int FUCKING_BIGBYTE[]; // line 63
extern const unsigned char FUCKING_PC1[]; // line 65
extern const unsigned char FUCKING_PC2[]; // line 66
extern const unsigned char FUCKING_DF_KEY[]; // line 72

#endif // SECSVR_INC_D3DES_H_H_
