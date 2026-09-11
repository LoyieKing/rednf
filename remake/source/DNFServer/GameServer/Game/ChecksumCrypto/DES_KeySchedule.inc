// ORIG 0x080abcbc：keyLen==8 否则 0x1002；弱/半弱密钥 0x1006；
// 生成 16 轮 × 8 字节子密钥至 alg->rk（返回错误码，DES3_KeySchedule 检查）
int DES_KeySchedule(unsigned char* key, unsigned long keyLen,
                    DES_ALG_INFO* alg)
{
    if (keyLen != 8)
        return AES_ERR_BAD_KEYLEN;
    for (int i = 0; i < 16; ++i) {
        if (std::memcmp(key, k_desWeakKeys[i], 8) == 0)
            return 0x1006;
    }
    unsigned long long k56 = desPermute(desLoad64(key), 64, 56, k_desPC1);
    unsigned long C = (unsigned long)(k56 >> 28);
    unsigned long D = (unsigned long)(k56 & 0x0fffffffUL);
    for (int r = 0; r < 16; ++r) {
        C = desRotl28(C, k_desShifts[r]);
        D = desRotl28(D, k_desShifts[r]);
        unsigned long long cd = ((unsigned long long)C << 28) | D;
        unsigned long long k48 = desPermute(cd, 56, 48, k_desPC2);
        for (int i = 0; i < 6; ++i)
            alg->rk[r * 8 + i] = (unsigned char)((k48 >> (40 - i * 8)) & 0xff);
        alg->rk[r * 8 + 6] = 0;
        alg->rk[r * 8 + 7] = 0;
    }
    return 0;
}