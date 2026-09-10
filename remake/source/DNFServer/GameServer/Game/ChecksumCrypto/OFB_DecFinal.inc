int OFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a4641（46 条）：pNum/buffered 具名局部 + *outLen 两写（ORIG 形态）。
    unsigned long* pNum = &alg->num;
    unsigned long buffered = alg->buffered;
    *outLen = buffered;
    AES_Encrypt(pNum, alg->chain);
    for (unsigned long k = 0; k < buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = buffered;
    return 0;
