int OFB_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG（与 AES 版同构）：rk/buffered 具名局部 + *outLen 两写。
    unsigned char* rk = alg->rk;
    unsigned long buffered = alg->buffered;
    *outLen = buffered;
    DES3_Encrypt(rk, alg->chain);
    for (unsigned long k = 0; k < buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = buffered;
    return 0;
