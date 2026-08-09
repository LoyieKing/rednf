// Auto-generated header stub from DWARF info
// Original path: inc/twofish.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_TWOFISH_H_H_
#define SECSVR_INC_TWOFISH_H_H_

#include "inc/publish/TeaMacro.h"

typedef unsigned int DWORD;
// sizeof = 24
struct CTwofish { // line 74
    // sizeof = 292
    struct ._38 {
    public:
    BYTE direction;
    int keyLen;
    unsigned char keyMaterial[];
    DWORD keySig;
    int numRounds;
    DWORD key32[];
    DWORD sboxKeys[];
    DWORD subKeys[];
    };
    // sizeof = 40
    struct ._39 {
    public:
    BYTE mode;
    BYTE IV[];
    DWORD cipherSig;
    DWORD iv32[];
    };
private:
unsigned char *P8x8[];
long unsigned int *MDStab[];
long unsigned int *_sBox_[];
unsigned char *numRounds;
void *m_pSys;
void *m_pLocal;
public:
CTwofish(); // line 106
~CTwofish(); // line 107
void Init(void *arg0, void *arg1); // line 109
void UnInit(); // line 110
void Twofish16(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3); // line 112
void Twofish24(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3); // line 114
void Twofish32(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3); // line 116
private:
int makeKey(CTwofish::._38 *arg0, BYTE arg1, int arg2, unsigned char *arg3); // line 132
int cipherInit(CTwofish::._39 *arg0, BYTE arg1, unsigned char *arg2); // line 134
int blockEncrypt(CTwofish::._39 *arg0, CTwofish::._38 *arg1, BYTE *arg2, int arg3, BYTE *arg4); // line 136
int blockDecrypt(CTwofish::._39 *arg0, CTwofish::._38 *arg1, BYTE *arg2, int arg3, BYTE *arg4); // line 138
int reKey(CTwofish::._38 *arg0); // line 141
int setRounds(int arg0, int arg1); // line 144
void BuildMDS(); // line 146
void ReverseRoundSubkeys(CTwofish::._38 *arg0, BYTE arg1); // line 148
};
namespace CTwofish {
    // sizeof = 292
    struct ._38 { // line 79
    public:
    BYTE direction;
    int keyLen;
    unsigned char keyMaterial[];
    DWORD keySig;
    int numRounds;
    DWORD key32[];
    DWORD sboxKeys[];
    DWORD subKeys[];
    };
    // sizeof = 40
    struct ._39 { // line 96
    public:
    BYTE mode;
    BYTE IV[];
    DWORD cipherSig;
    DWORD iv32[];
    };
    // sizeof = 292
    struct ._30 { // line 79
    public:
    BYTE direction;
    int keyLen;
    unsigned char keyMaterial[];
    DWORD keySig;
    int numRounds;
    DWORD key32[];
    DWORD sboxKeys[];
    DWORD subKeys[];
    };
    // sizeof = 40
    struct ._31 { // line 96
    public:
    BYTE mode;
    BYTE IV[];
    DWORD cipherSig;
    DWORD iv32[];
    };
} // namespace CTwofish


extern DWORD RS_MDS_Encode(DWORD k0, DWORD k1); // line 423 (inferred)
extern int ParseHexDword(int bits, const unsigned char *srcTxt, DWORD *d, unsigned char *dstTxt); // line 378 (inferred)
extern void Xor256(void *dst, void *src, BYTE b); // line 557 (inferred)

#endif // SECSVR_INC_TWOFISH_H_H_
