void AES_EncKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24 && keyLen != 32)
        return;                      // ORIG 返错误码 0x1002 的 void 口径
    alg->num = (keyLen + 3) >> 2;
    RIJNDAEL_KeySchedule(key, keyLen, alg->rk);
}