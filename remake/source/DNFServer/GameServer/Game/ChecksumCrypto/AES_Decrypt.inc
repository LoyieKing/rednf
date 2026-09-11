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
    std::memcpy(state, block, 16);
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
    std::memcpy(block, state, 16);
}