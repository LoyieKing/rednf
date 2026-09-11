int CBC_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080b1255（与 AES 版同构）：原地解密 out，链更新为 buf，
    // buffered==0 → 空尾；buffered!=blockLen → 0x1005。
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