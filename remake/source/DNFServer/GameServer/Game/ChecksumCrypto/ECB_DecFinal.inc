int ECB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a4396（80 条）：buffered==0 → 空尾；!=blockLen → 0x1005；
    // 原地解密 out（密文取自 buf），PaddCheck 返回值按 ORIG 与 -3/-1 比较。
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
    unsigned long pad = PaddCheck(out, blockLen, alg->padding);
    if (pad == 0xfffffffd)
        return 0x1003;
    if (pad == 0xffffffff)
        return 0x1001;
    *outLen = blockLen - pad;
    return 0;
}