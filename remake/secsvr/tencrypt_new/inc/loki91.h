// Auto-generated header stub from DWARF info（类型名清理）
// Original path: inc/loki91.h
#ifndef SECSVR_INC_LOKI91_H_H_
#define SECSVR_INC_LOKI91_H_H_

struct sfn_desc91;

// sizeof = 28
struct CLoki91 { // line 12
private:
unsigned int lokikey[2];
unsigned char *P;
unsigned int *subkeys;
sfn_desc91 *sfn;
void *m_pSys;
void *m_pLocal;
public:
CLoki91(); // line 14
~CLoki91(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void Loki91(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
private:
void blockEncrypt(unsigned char *arg0); // line 34
void blockDecrypt(unsigned char *arg0); // line 35
void setkey(unsigned char *arg0); // line 36
unsigned short s(unsigned int arg0); // line 38
void perm32(unsigned int *arg0, unsigned int *arg1, unsigned char *arg2); // line 39
unsigned int f(unsigned int arg0, unsigned int arg1); // line 40
unsigned short mult8(unsigned short arg0, unsigned short arg1, unsigned short arg2); // line 41
unsigned short exp_8(unsigned short arg0, unsigned short arg1, unsigned short arg2); // line 42
};

#endif // SECSVR_INC_LOKI91_H_H_
