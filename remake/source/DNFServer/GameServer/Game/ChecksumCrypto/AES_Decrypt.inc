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
