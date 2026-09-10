int CFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a46c7（81 条）：keystream 就地回写 chain（4×ulong XOR）后 memcpy 出。
    unsigned long* pNum = &alg->num;
    unsigned long buffered = alg->buffered;
    *outLen = buffered;
    AES_Encrypt(pNum, alg->chain);
    ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->chain)[0] ^ ((unsigned long*)alg->buf)[0];
    ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->chain)[1] ^ ((unsigned long*)alg->buf)[1];
    ((unsigned long*)alg->chain)[2] = ((unsigned long*)alg->chain)[2] ^ ((unsigned long*)alg->buf)[2];
    ((unsigned long*)alg->chain)[3] = ((unsigned long*)alg->chain)[3] ^ ((unsigned long*)alg->buf)[3];
    std::memcpy(out, alg->chain, buffered);
    *outLen = buffered;
    return 0;
