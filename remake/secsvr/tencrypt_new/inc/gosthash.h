/* gosthash.h -- GOST R 34.11-94（Saarinen 参考实现）的 tencrypt_new 变体
   由 DWARF 重建：CGostHash（sizeof 20 = 5 指针）+ 上下文
   （sizeof 132 = sum[8]+hash[8]+len[8]+partial[32]+partial_bytes）。 */

#ifndef TENCRYPT_INC_GOSTHASH_H_
#define TENCRYPT_INC_GOSTHASH_H_

class CGostHash {
public:
    CGostHash();
    ~CGostHash();
    void InitBoxData();
    void gosthash_reset();
    void gosthash_update(unsigned char *buf, unsigned int len);
    void gosthash_final(unsigned char *digest);
private:
    struct gost_ctx {
        unsigned int sum[8];        /* 32 */
        unsigned int hash[8];       /* 32 */
        unsigned int len[8];        /* 32 */
        unsigned char partial[32];  /* 32 */
        unsigned int partial_bytes; /* 4 */
    };
    unsigned int *m_psbox_1;
    unsigned int *m_psbox_2;
    unsigned int *m_psbox_3;
    unsigned int *m_psbox_4;
    gost_ctx *m_pctx;

    void gosthash_compress(unsigned int *h, unsigned int *m);
    void gosthash_bytes(unsigned char *buf, unsigned int bits);
};

#endif /* TENCRYPT_INC_GOSTHASH_H_ */
