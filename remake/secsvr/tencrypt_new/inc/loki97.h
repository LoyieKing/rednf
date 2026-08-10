/* loki97.h -- LOKI97 块密码（tencrypt 复刻）
   类型布局按 DWARF：keyInstance sizeof 460、cipherInstance sizeof 40、
   CLoki97 sizeof 32（S1/S2/DELTA/P/init_done/m_pSys/m_pLocal）。 */
#ifndef SECSVR_INC_LOKI97_H_H_
#define SECSVR_INC_LOKI97_H_H_

typedef struct { unsigned int l, r; } loki_B64;

struct CLoki97 {
    struct keyInstance {            // sizeof = 460
        unsigned char direction;    // +0x00
        int keyLen;                 // +0x04
        unsigned char keyMaterial[65]; // +0x08
        loki_B64 SK[48];            // +0x4c
    };
    struct cipherInstance {         // sizeof = 40
        unsigned char mode;         // +0x00
        unsigned char IV[16];       // +0x01
        loki_B64 IVL;               // +0x14
        loki_B64 IVR;               // +0x1c
        int blockSize;              // +0x24
    };
private:
    unsigned char *S1;
    unsigned char *S2;
    loki_B64 DELTA;
    loki_B64 *P;
    int init_done;
    void *m_pSys;
    void *m_pLocal;
public:
    CLoki97();                       // line 33
    ~CLoki97();                      // line 34
    void Init(void *pSys, void *pLocal);  // line 36
    void UnInit();                   // line 37
    void Loki97(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen);     // line 39
private:
    unsigned char *ULONG64ToBYTE(unsigned char *buf, loki_B64 I);  // line 65
    loki_B64 byteToULONG64(unsigned char *inp);                    // line 66
    loki_B64 sub64(loki_B64 a, loki_B64 b);                        // line 67
    loki_B64 add64(loki_B64 a, loki_B64 b);                        // line 68
    loki_B64 f(loki_B64 A, loki_B64 B);                            // line 69
    int deCFB1(cipherInstance *cipher, keyInstance *key,
               unsigned char *input, int inputLen,
               unsigned char *outBuffer);                          // line 71
    int deCBC(cipherInstance *cipher, keyInstance *key,
              unsigned char *input, int inputLen,
              unsigned char *outBuffer);                           // line 73
    int deECB(cipherInstance *cipher, keyInstance *key,
              unsigned char *input, int inputLen,
              unsigned char *outBuffer);                           // line 75
    int blockDecrypt(cipherInstance *cipher, keyInstance *key,
                     unsigned char *input, int inputLen,
                     unsigned char *outBuffer);                    // line 77
    int enCFB1(cipherInstance *cipher, keyInstance *key,
               unsigned char *input, int inputLen,
               unsigned char *outBuffer);                          // line 79
    int enCBC(cipherInstance *cipher, keyInstance *key,
              unsigned char *input, int inputLen,
              unsigned char *outBuffer);                           // line 81
    int enECB(cipherInstance *cipher, keyInstance *key,
              unsigned char *input, int inputLen,
              unsigned char *outBuffer);                           // line 83
    int blockEncrypt(cipherInstance *cipher, keyInstance *key,
                     unsigned char *input, int inputLen,
                     unsigned char *outBuffer);                    // line 85
    int makeKey(keyInstance *key, unsigned char direction,
                int keyLen, unsigned char *keyMaterial);           // line 86
    int mult(int a, int b, int g, int n);                          // line 87
    int exp3(int b, int g, int n);                                 // line 88
    int cipherInit(cipherInstance *cipher, unsigned char mode,
                   unsigned char *IV);                             // line 89
};

#endif // SECSVR_INC_LOKI97_H_H_
