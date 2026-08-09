// Auto-generated header stub from DWARF info
// Original path: inc/fastdes.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_FASTDES_H_H_
#define SECSVR_INC_FASTDES_H_H_

// sizeof = 8
struct CFastDes { // line 12
    // sizeof = 8
    struct keystage {
    public:
    unsigned int h;
    unsigned int l;
    };
    // sizeof = 128
    struct keysched {
    public:
    CFastDes::keystage KS[];
    };
private:
void *m_pSys;
void *m_pLocal;
static const unsigned char FUCKING_PRESHIFT[];
static const unsigned int hKS_C4[][];
static const unsigned int lKS_D4[][];
static const unsigned int wL_I8[];
static const unsigned int wO_L4[];
static const unsigned int wPS[][];
static const unsigned int wC_K4[][];
static const unsigned int wC_K3[][];
static const unsigned int wD_K4[][];
static const unsigned int wD_K3[][];
public:
CFastDes(); // line 14
~CFastDes(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void FastDes(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
private:
void fencrypt(unsigned char *arg0, int arg1, CFastDes::keysched *arg2); // line 31
void fsetkey(unsigned char *arg0, CFastDes::keysched *arg1); // line 32
};
extern const unsigned char FUCKING_PRESHIFT[]; // line 45
extern const unsigned int hKS_C4[][]; // line 71
extern const unsigned int lKS_D4[][]; // line 72
extern const unsigned int wL_I8[]; // line 74
extern const unsigned int wO_L4[]; // line 75
extern const unsigned int wPS[][]; // line 76
extern const unsigned int wC_K4[][]; // line 78
extern const unsigned int wC_K3[][]; // line 79
extern const unsigned int wD_K4[][]; // line 80
extern const unsigned int wD_K3[][]; // line 81
namespace CFastDes {
    // sizeof = 8
    struct keystage { // line 23
    public:
    unsigned int h;
    unsigned int l;
    };
    // sizeof = 128
    struct keysched { // line 27
    public:
    CFastDes::keystage KS[];
    };
} // namespace CFastDes

#endif // SECSVR_INC_FASTDES_H_H_
