int CBC_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a448d：原地解密 out（密文取自 alg->buf），链更新为 buf；
    // buffered==0 → 空尾；buffered!=blockLen → 0x1005（缺块）；
    // PaddCheck 返回值按 ORIG 原码与 -3/-1 比较（0x1003/0x1001 映射）。
    unsigned long* pNum = &alg->num;
    unsigned long blockLen = 16;
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
    ((unsigned long*)out)[2] = ((unsigned long*)alg->buf)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)alg->buf)[3];
    AES_Decrypt(pNum, out);
    ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->chain)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->chain)[1];
    ((unsigned long*)out)[2] = ((unsigned long*)out)[2] ^ ((unsigned long*)alg->chain)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)out)[3] ^ ((unsigned long*)alg->chain)[3];
    ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->buf)[1];
    ((unsigned long*)alg->chain)[2] = ((unsigned long*)alg->buf)[2];
    ((unsigned long*)alg->chain)[3] = ((unsigned long*)alg->buf)[3];
    unsigned long pad = PaddCheck(out, blockLen, alg->padding);
    if (pad == 0xfffffffd)
        return 0x1003;
    if (pad == 0xffffffff)
        return 0x1001;
    *outLen = blockLen - pad;
    return 0;
