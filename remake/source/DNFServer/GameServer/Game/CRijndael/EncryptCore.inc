void CRijndael::EncryptCore(const unsigned char* in, unsigned char* out)
{
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    const int* sh = kShifts[(nb - 4) / 2];
    unsigned char state[4][8];

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = in[r + c * 4];

    AddRoundKey(state, 0);

    for (int round = 1; round < nr; ++round)
    {
        // SubBytes
        for (int c = 0; c < nb; ++c)
            for (int r = 0; r < 4; ++r)
                state[r][c] = kSBox[state[r][c]];

        // ShiftRows（行 r 左移 sh[r-1]）
        for (int r = 1; r < 4; ++r)
        {
            unsigned char tmp[8];
            int s = sh[r - 1];
            for (int c = 0; c < nb; ++c)
                tmp[c] = state[r][(c + s) % nb];
            for (int c = 0; c < nb; ++c)
                state[r][c] = tmp[c];
        }

        // MixColumns
        for (int c = 0; c < nb; ++c)
        {
            unsigned char a0 = state[0][c], a1 = state[1][c];
            unsigned char a2 = state[2][c], a3 = state[3][c];
            state[0][c] = (unsigned char)(GfMul(a0, 2) ^ GfMul(a1, 3) ^ a2 ^ a3);
            state[1][c] = (unsigned char)(a0 ^ GfMul(a1, 2) ^ GfMul(a2, 3) ^ a3);
            state[2][c] = (unsigned char)(a0 ^ a1 ^ GfMul(a2, 2) ^ GfMul(a3, 3));
            state[3][c] = (unsigned char)(GfMul(a0, 3) ^ a1 ^ a2 ^ GfMul(a3, 2));
        }

        AddRoundKey(state, round);
    }

    // 最后一轮
    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = kSBox[state[r][c]];
    for (int r = 1; r < 4; ++r)
    {
        unsigned char tmp[8];
        int s = sh[r - 1];
        for (int c = 0; c < nb; ++c)
            tmp[c] = state[r][(c + s) % nb];
        for (int c = 0; c < nb; ++c)
            state[r][c] = tmp[c];
    }
    AddRoundKey(state, nr);

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            out[r + c * 4] = state[r][c];
