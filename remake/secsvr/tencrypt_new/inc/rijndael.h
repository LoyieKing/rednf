/* rijndael.h -- Rijndael/AES 块密码（tencrypt 复刻）
   类型布局按 DWARF/二进制：CRijndael sizeof 528 =
   m_Context(488) + dtbl/itbl/sbox/isbox/Logtable/Alogtable(6 指针)
   + idx/iidx(2 指针) + m_pSys/m_pLocal。 */
#ifndef SECSVR_INC_RIJNDAEL_H_H_
#define SECSVR_INC_RIJNDAEL_H_H_

struct CRijndael {
    struct tagRIJNDAELContext {     // sizeof = 488
        unsigned int keys[60];      // +0x000
        unsigned int ikeys[60];     // +0x0f0
        int nrounds;                // +0x1e0
        int mode;                   // +0x1e4
    };
private:
    tagRIJNDAELContext m_Context;   // +0x000
    unsigned int *dtbl;             // +0x1e8
    unsigned int *itbl;             // +0x1ec
    unsigned char *sbox;            // +0x1f0
    unsigned char *isbox;           // +0x1f4
    unsigned char *Logtable;        // +0x1f8
    unsigned char *Alogtable;       // +0x1fc
    unsigned int *idx;              // +0x200
    unsigned int *iidx;             // +0x204
    void *m_pSys;                   // +0x208
    void *m_pLocal;                 // +0x20c
public:
    CRijndael();                    // line 25
    ~CRijndael();                   // line 26
    void Init(void *pSys, void *pLocal);    // line 27
    void UnInit();                  // line 28
    void RijnDael(unsigned char *key, bool benc, unsigned char *buf,
                  unsigned int buflen);    // line 29
private:
    void rijndael_setup(tagRIJNDAELContext *ctx, unsigned int keysize,
                        const unsigned char *key);      // line 42
    void rijndael_encrypt(tagRIJNDAELContext *ctx,
                          const unsigned char *plaintext,
                          unsigned char *ciphertext);   // line 44
    void rijndael_decrypt(tagRIJNDAELContext *ctx,
                          const unsigned char *ciphertext,
                          unsigned char *plaintext);    // line 46
    void block_encrypt(tagRIJNDAELContext *ctx, unsigned char *input,
                       int inputlen, unsigned char *output,
                       unsigned char *iv);              // line 48
    void block_decrypt(tagRIJNDAELContext *ctx, unsigned char *input,
                       int inputlen, unsigned char *output,
                       unsigned char *iv);              // line 50
    void SetKey(unsigned int keysize, const unsigned char *key); // line 52
    void Encrypt(const unsigned char *in, unsigned char *out);   // line 56
    void Decrypt(const unsigned char *in, unsigned char *out);   // line 60
    void BlockEncrypt(unsigned char *in, int inputlen,
                      unsigned char *out, unsigned char *iv);    // line 65
    void BlockDecrypt(unsigned char *in, int inputlen,
                      unsigned char *out, unsigned char *iv);    // line 69
    void Initialize(int nrounds, int mode);             // line 74
    unsigned char xtime(unsigned char a);               // line 76
    unsigned char mul(unsigned char a, unsigned char b); // line 78
    void inv_mix_column(unsigned int *a, unsigned int *b); // line 80
    void key_addition_8to32(const unsigned char *txt,
                            unsigned int *keys, unsigned int *out); // line 82
    void key_addition32(const unsigned int *txt,
                        unsigned int *keys, unsigned int *out); // line 84
    void key_addition32to8(const unsigned int *txt,
                           unsigned int *keys, unsigned char *out); // line 86
};

#endif // SECSVR_INC_RIJNDAEL_H_H_
