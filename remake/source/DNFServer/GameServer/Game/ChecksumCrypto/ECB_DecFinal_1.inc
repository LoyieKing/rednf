int ECB_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG（与 AES 版同构，blockLen=8）：buffered==0 → 空尾；!=blockLen → 0x1005；
    // 原地解密 out，PaddCheck 返回值按 ORIG 与 -3/-1 比较（旧实现错误码映射错）。
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
    DES_Decrypt(rk, out);
    unsigned long pad = PaddCheck(out, blockLen, alg->padding);
    if (pad == 0xfffffffd)
        return 0x1003;
    if (pad == 0xffffffff)
        return 0x1001;
    *outLen = blockLen - pad;
    return 0;
}