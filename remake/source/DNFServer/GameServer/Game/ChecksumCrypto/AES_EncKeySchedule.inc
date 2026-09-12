void AES_EncKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24 && keyLen != 32)
        return;                      // ORIG 返错误码 0x1002 的 void 口径
    // [SemA03 harness 实测] ORIG 以 32 位字反序装载 key（实测 ORIG ek 第二组
    //   = 76ffd5a972fad3ae… vs 标准 d6aa74fdd2af72fa…；端到端等价式见
    //   AES_Encrypt.inc）。此处按字反序后交给标准扩展，使整链与 ORIG 一致。
    unsigned char kb[32];
    for (unsigned long i = 0; i < keyLen; i += 4)
        for (int j = 0; j < 4; ++j)
            kb[i + j] = key[i + (3 - j)];
    alg->num = (keyLen + 3) >> 2;
    RIJNDAEL_KeySchedule(kb, keyLen, alg->rk);
}