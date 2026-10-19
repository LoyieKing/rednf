// Auto-generated header stub from DWARF info
// Original path: inc/loki97.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_LOKI97_H_H_
#define SECSVR_INC_LOKI97_H_H_

// sizeof = 88
struct ._33 { // line 9
public:
unsigned int state[];
unsigned int count[];
unsigned char buffer[];
};
// sizeof = 32
struct CLoki97 { // line 31
    // sizeof = 460
    struct ._34 {
    public:
    unsigned char direction;
    int keyLen;
    unsigned char keyMaterial[];
    ._33 SK[];
    };
    // sizeof = 40
    struct ._35 {
    public:
    unsigned char mode;
    unsigned char IV[];
    ._33 IVL;
    ._33 IVR;
    int blockSize;
    };
private:
unsigned char *S1;
unsigned char *S2;
._33 DELTA;
._33 *P;
int init_done;
void *m_pSys;
void *m_pLocal;
public:
CLoki97(); // line 33
~CLoki97(); // line 34
void Init(void *arg0, void *arg1); // line 36
void UnInit(); // line 37
void Loki97(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 39
private:
unsigned char * ULONG64ToBYTE(unsigned char *arg0, ._33 arg1); // line 65
._33 byteToULONG64(unsigned char *arg0); // line 66
._33 sub64(._33 arg0, ._33 arg1); // line 67
._33 add64(._33 arg0, ._33 arg1); // line 68
._33 f(._33 arg0, ._33 arg1); // line 69
int deCFB1(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 71
int deCBC(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 73
int deECB(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 75
int blockDecrypt(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 77
int enCFB1(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 79
int enCBC(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 81
int enECB(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 83
int blockEncrypt(CLoki97::._35 *arg0, CLoki97::._34 *arg1, unsigned char *arg2, int arg3, unsigned char *arg4); // line 85
int makeKey(CLoki97::._34 *arg0, unsigned char arg1, int arg2, unsigned char *arg3); // line 86
int mult(int arg0, int arg1, int arg2, int arg3); // line 87
int exp3(int arg0, int arg1, int arg2); // line 88
int cipherInit(CLoki97::._35 *arg0, unsigned char arg1, unsigned char *arg2); // line 89
};
namespace CLoki97 {
    // sizeof = 460
    struct ._34 { // line 44
    public:
    unsigned char direction;
    int keyLen;
    unsigned char keyMaterial[];
    ._33 SK[];
    };
    // sizeof = 40
    struct ._35 { // line 54
    public:
    unsigned char mode;
    unsigned char IV[];
    ._33 IVL;
    ._33 IVR;
    int blockSize;
    };
    // sizeof = 460
    struct ._31 { // line 44
    public:
    unsigned char direction;
    int keyLen;
    unsigned char keyMaterial[];
    ._30 SK[];
    };
    // sizeof = 40
    struct ._32 { // line 54
    public:
    unsigned char mode;
    unsigned char IV[];
    ._30 IVL;
    ._30 IVR;
    int blockSize;
    };
} // namespace CLoki97

#endif // SECSVR_INC_LOKI97_H_H_
