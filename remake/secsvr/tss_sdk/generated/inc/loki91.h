// Auto-generated header stub from DWARF info
// Original path: inc/loki91.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_LOKI91_H_H_
#define SECSVR_INC_LOKI91_H_H_

// sizeof = 28
struct CLoki91 { // line 12
    // sizeof = 4
    struct ._37 {
    public:
    short unsigned int gen;
    short unsigned int exp;
    };
private:
unsigned int lokikey[];
unsigned char *P;
unsigned int *subkeys;
CLoki91::._37 *sfn;
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
short unsigned int s(unsigned int arg0); // line 38
void perm32(unsigned int *arg0, unsigned int *arg1, unsigned char *arg2); // line 39
unsigned int f(unsigned int arg0, unsigned int arg1); // line 40
short unsigned int mult8(short unsigned int arg0, short unsigned int arg1, short unsigned int arg2); // line 41
short unsigned int exp_8(short unsigned int arg0, short unsigned int arg1, short unsigned int arg2); // line 42
};
namespace CLoki91 {
    // sizeof = 4
    struct ._37 { // line 27
    public:
    short unsigned int gen;
    short unsigned int exp;
    };
    // sizeof = 4
    struct ._30 { // line 27
    public:
    short unsigned int gen;
    short unsigned int exp;
    };
} // namespace CLoki91

#endif // SECSVR_INC_LOKI91_H_H_
