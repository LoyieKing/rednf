int CBC_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080af7bd（与 DES 版逐条同构，仅 DES3_Decrypt）。
    unsigned char* rk = alg->rk;
    unsigned long blockLen = 8;
    unsigned long buffered = alg->buffered;
    if (buffered == 0) {
        *outLen = 0;
        return 0;
    }
    *outLen = blockLen;
    if (buffered != blockLen)
        return 0x1005;
    ((unsigned long*)out)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->buf)[1];
    DES3_Decrypt(rk, out);
    ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->chain)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->chain)[1];
    ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->buf)[1];
    unsigned long pad = PaddCheck(out, blockLen, alg->padding);
    if (pad == 0xfffffffd)
        return 0x1003;
    if (pad == 0xffffffff)
        return 0x1001;
    *outLen = blockLen - pad;
    return 0;
}