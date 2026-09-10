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
