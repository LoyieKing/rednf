int CFB_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG（与 AES 版同构）：keystream 就地回写 chain（2×ulong XOR）后 memcpy 出。
    unsigned char* rk = alg->rk;
    unsigned long buffered = alg->buffered;
    *outLen = buffered;
    DES3_Encrypt(rk, alg->chain);
    ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->chain)[0] ^ ((unsigned long*)alg->buf)[0];
    ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->chain)[1] ^ ((unsigned long*)alg->buf)[1];
    std::memcpy(out, alg->chain, buffered);
    *outLen = buffered;
    return 0;
}