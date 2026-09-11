// ORIG AES_Encrypt(void*, unsigned char*) @0x0809f981：完全展开单块加密。
// 参数语义：pNumArea = &alg->num（结构体 +0x3c），ek 紧随其后（+0x40）。
// [b28d-41] 豁免（用户规则 §6：AES/DES/SHA 通用算法类只保算法正确，不强制
// identical）：ORIG 4.1 将 10/12/14 轮完全展开（1595 条），重建按轮循环表达
// （70 条）——语义等价、轮数由 num+6 驱动（AES-128/192/256 通用）。
// 实测（/tmp/d41_crypto_test.cpp，链接 tu_ChecksumCrypto.o 运行）：AES-128
// FIPS-197 向量 key 000102…0f / pt 00112233…ff → ct 69c4e0d8…b4c55a ✓、
// AES enc/dec roundtrip ✓（本轮修 AES_DecKeySchedule 轮数真 bug，见该函数）。
// [b28d-solo-ChecksumCrypto] AES_Encrypt N 封顶（算法族豁免，guide §6）：ORIG 为
// _ZL6ft_tab 表驱动 10/12/14 轮完全展开（1595 条，报告 0x0809f981）；重建为
// SBox 表 + 轮循环（70 条）。实现策略不同 ⇒ 源形不可复现；FIPS-197 向量 ✓。
void AES_Encrypt(void* pNumArea, unsigned char* block)
{
    unsigned long const* pNum = reinterpret_cast<unsigned long const*>(pNumArea);
    unsigned long num = pNum[0];
    unsigned long const* rk = pNum + 1;
    unsigned long nr = num + 6;      // Nb=4：Nk(4/6/8)+6 → 10/12/14 轮

    unsigned char state[16];
    std::memcpy(state, block, 16);
    aesAddRoundKey(state, rk);
    for (unsigned long r = 1; r < nr; ++r) {
        aesSubBytes(state);
        aesShiftRows(state);
        aesMixColumns(state);
        aesAddRoundKey(state, rk + r * 4);
    }
    aesSubBytes(state);
    aesShiftRows(state);
    aesAddRoundKey(state, rk + nr * 4);
    std::memcpy(block, state, 16);
}