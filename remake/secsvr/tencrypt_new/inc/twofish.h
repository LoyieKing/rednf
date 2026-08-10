// twofish.h -- CTwofish（Tencent TSS SDK tencrypt_new 复刻）
// 类/嵌套结构布局按 secagent 二进制 DWARF + 反汇编精确还原：
//   keyInstance    sizeof=292：direction@0 keyLen@4 keyMaterial[65]@8 keySig@0x4c
//                  numRounds@0x50 key32[8]@0x54 sboxKeys[4]@0x74 subKeys[40]@0x84
//   cipherInstance sizeof=40：mode@0 IV[16]@1 cipherSig@0x14 iv32[4]@0x18
//   CTwofish      sizeof=24：P8x8@0 MDStab@4 _sBox_@8 numRounds@0xc m_pSys@0x10 m_pLocal@0x14
// 嵌套结构原名 keyInstance/cipherInstance（二进制 mangled 名确认，非 DWARF 的 ._38/._39）。
#ifndef SECSVR_INC_TWOFISH_H_H_
#define SECSVR_INC_TWOFISH_H_H_

#include "inc/publish/TeaMacro.h"

typedef unsigned int DWORD;

struct CTwofish {
    struct keyInstance {                      // sizeof = 292
        BYTE direction;                       // 0: 0=encrypt 1=decrypt
        int keyLen;                           // 4: bits (128/192/256)
        unsigned char keyMaterial[65];        // 8..0x48 (hex 文本 + NUL)
        DWORD keySig;                         // 0x4c
        int numRounds;                        // 0x50
        DWORD key32[8];                       // 0x54
        DWORD sboxKeys[4];                    // 0x74
        DWORD subKeys[40];                    // 0x84..0x123
    };
    struct cipherInstance {                   // sizeof = 40
        BYTE mode;                            // 0: 1=ECB 2=CBC 3=CFB1
        BYTE IV[16];                          // 1..0x10
        DWORD cipherSig;                      // 0x14
        DWORD iv32[4];                        // 0x18..0x27
    };
private:
    unsigned char *P8x8;                      // 0: P[0]+P[1] 512B
    DWORD *MDStab;                            // 4: 4x256 DWORD
    DWORD *_sBox_;                            // 8: 2x512 DWORD（c0/c1 交错）
    unsigned char *numRounds;                 // 12: [4] = {0,16,16,16}
    void *m_pSys;                             // 16
    void *m_pLocal;                           // 20
public:
    CTwofish();
    ~CTwofish();
    void Init(void *arg0, void *arg1);
    void UnInit();
    void Twofish16(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3);
    void Twofish24(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3);
    void Twofish32(unsigned char *arg0, bool arg1, unsigned char *arg2, long unsigned int arg3);
private:
    int makeKey(keyInstance *arg0, BYTE arg1, int arg2, unsigned char *arg3);
    int cipherInit(cipherInstance *arg0, BYTE arg1, unsigned char *arg2);
    int blockEncrypt(cipherInstance *arg0, keyInstance *arg1, BYTE *arg2, int arg3, BYTE *arg4);
    int blockDecrypt(cipherInstance *arg0, keyInstance *arg1, BYTE *arg2, int arg3, BYTE *arg4);
    int reKey(keyInstance *arg0);
    int setRounds(int arg0, int arg1);
    void BuildMDS();
    void ReverseRoundSubkeys(keyInstance *arg0, BYTE arg1);
};

extern DWORD RS_MDS_Encode(DWORD k0, DWORD k1);
extern int ParseHexDword(int bits, const unsigned char *srcTxt, DWORD *d, unsigned char *dstTxt);
extern void Xor256(void *dst, void *src, BYTE b);

#endif // SECSVR_INC_TWOFISH_H_H_
