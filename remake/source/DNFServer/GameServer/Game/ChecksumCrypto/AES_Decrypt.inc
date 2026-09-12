// [b28d-solo-ChecksumCrypto] AES_Decrypt N 封顶（算法族豁免，guide §6）：ORIG 为
// _ZL6ft_tab 表驱动 10/12/14 轮完全展开（1595 条，报告 0x0809f981 同族）；重建为
// InvSBox 表 + 轮循环（70 条）。AES roundtrip ✓（/tmp/d41_crypto_test.cpp）。
void AES_Decrypt(void* pNumArea, unsigned char* block)
{
    unsigned long const* pNum = reinterpret_cast<unsigned long const*>(pNumArea);
    unsigned long num = pNum[0];
    unsigned long const* dk = pNum + 1;
    unsigned long nr = num + 6;

    unsigned char state[16];
    // [SemA03+SemA02 2026-09-12 harness 实测] ORIG 的 AES 是「每 4 字节字反序」
    // 变体（SemA03 对 AES_Encrypt 实测等价式 ORIG(pt)=bswap32(std(bswap32(key),
    // bswap32(pt)))）；解密侧同轴：state 按 32 位字反序装入、回写时再反序。
    // 实测依据：/tmp/aes_run32（mmap ORIG 0x080a0ffe）vs 本实现同输入对比。
    for (int w = 0; w < 4; ++w)
        for (int j = 0; j < 4; ++j)
            state[w * 4 + j] = block[w * 4 + (3 - j)];
    aesAddRoundKey(state, dk);
    for (unsigned long r = 1; r < nr; ++r) {
        aesInvSubBytes(state);
        aesInvShiftRows(state);
        aesInvMixColumnsBlk(state);              // dk 已做 InvMixColumn 预变换，逆列混合须先于加轮密钥
        aesAddRoundKey(state, dk + r * 4);
    }
    aesInvSubBytes(state);
    aesInvShiftRows(state);
    aesAddRoundKey(state, dk + nr * 4);
    for (int w = 0; w < 4; ++w)
        for (int j = 0; j < 4; ++j)
            block[w * 4 + j] = state[w * 4 + (3 - j)];
}