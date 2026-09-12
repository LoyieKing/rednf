void AES_DecKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24 && keyLen != 32)
        return;
    alg->num = (keyLen + 3) >> 2;

    unsigned long esk[60];
    // [SemA03+SemA02 2026-09-12 harness 实测] ORIG 以 32 位字反序装载 key（与
    // AES_EncKeySchedule 同轴，见其同名注释）；解密侧必须同法反序，否则
    // Dec 与 Enc 的密钥调度不自洽（我方 Dec(Enc(pt)) 会错）。
    unsigned char kb[32];
    for (unsigned long i = 0; i < keyLen; i += 4)
        for (int j = 0; j < 4; ++j)
            kb[i + j] = key[i + (3 - j)];
    RIJNDAEL_KeySchedule(kb, keyLen, esk);
    // [b28d-41] 真 bug 修复：原写 `alg->num / 4 + 6`，对 AES-128（num=4）得 7 轮
    // （应为 Nk+6=10；AES-192/256 同理得 7/8 而非 12/14）⇒ 等效解密轮密钥
    // dk[0]=esk[nr*4] 取到 esk[28] 而非 esk[40]，AES 解密 roundtrip 全错。
    // 轮数与 AES_Encrypt/AES_Decrypt 一致应为 num + 6。
    unsigned long nr = alg->num + 6;

    // ORIG 等效解密轮密钥：dk[0]=esk[nr]，中间组 InvMixColumn，dk[nr]=esk[0]
    std::memcpy(&alg->rk[0], &esk[nr * 4], 16);
    for (unsigned long r = 1; r < nr; ++r) {
        unsigned char col[16];
        for (int c = 0; c < 4; ++c) {
            unsigned long w = esk[(nr - r) * 4 + c];
            col[c * 4 + 0] = (unsigned char)(w >> 24);
            col[c * 4 + 1] = (unsigned char)(w >> 16);
            col[c * 4 + 2] = (unsigned char)(w >> 8);
            col[c * 4 + 3] = (unsigned char)w;
        }
        aesInvMixColumnsBlk(col);
        for (int c = 0; c < 4; ++c) {
            alg->rk[r * 4 + c] =
                ((unsigned long)col[c * 4 + 0] << 24) |
                ((unsigned long)col[c * 4 + 1] << 16) |
                ((unsigned long)col[c * 4 + 2] << 8) |
                (unsigned long)col[c * 4 + 3];
        }
    }
    std::memcpy(&alg->rk[nr * 4], &esk[0], 16);
}