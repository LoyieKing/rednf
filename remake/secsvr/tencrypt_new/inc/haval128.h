/* haval128.h -- HAVAL V.1（3 轮，128 位）的 tencrypt_new 变体
   由 DWARF 重建：CHaval128 + 上下文（sizeof 208 = 2+2+32+128+4+8+32）。 */

#ifndef TENCRYPT_INC_HAVAL128_H_
#define TENCRYPT_INC_HAVAL128_H_

class CHaval128 {
public:
    CHaval128();
    ~CHaval128();
    void havalInit();
    int havalUpdate(unsigned char *dataBuffer, unsigned int dataLength);
    int havalFinal(unsigned char *digest);
private:
    struct haval_ctx {
        unsigned short passes;      /* 2 */
        unsigned short hashLength;  /* 2 */
        unsigned int digest[8];     /* 32 */
        unsigned char block[128];   /* 128 */
        unsigned int occupied;      /* 4 */
        unsigned int bitCount[2];   /* 8 */
        unsigned int temp[8];       /* 32 */
    };
    haval_ctx *m_pctx;

    void havalTransform3(unsigned int *E, unsigned char *D, unsigned int *T);
};

#endif /* TENCRYPT_INC_HAVAL128_H_ */
