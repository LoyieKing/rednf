void CRijndael::DecryptCore(const unsigned char* in, unsigned char* out)
{
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    const int* sh = kShifts[(nb - 4) / 2];
    unsigned char state[4][8];

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = in[r + c * 4];

    InvAddRoundKey(state, nr);

    for (int round = nr - 1; round >= 1; --round)
    {
        // InvShiftRows（行 r 右移 sh[r-1]）
        for (int r = 1; r < 4; ++r)
        {
            unsigned char tmp[8];
            int s = sh[r - 1];
            for (int c = 0; c < nb; ++c)
                tmp[c] = state[r][(c - s + nb) % nb];
            for (int c = 0; c < nb; ++c)
                state[r][c] = tmp[c];
        }

        // InvSubBytes
        for (int c = 0; c < nb; ++c)
            for (int r = 0; r < 4; ++r)
                state[r][c] = kSBoxInv[state[r][c]];

        InvAddRoundKey(state, round);

        // InvMixColumns
        for (int c = 0; c < nb; ++c)
        {
            unsigned char a0 = state[0][c], a1 = state[1][c];
            unsigned char a2 = state[2][c], a3 = state[3][c];
            state[0][c] = (unsigned char)(GfMul(a0, 14) ^ GfMul(a1, 11) ^ GfMul(a2, 13) ^ GfMul(a3, 9));
            state[1][c] = (unsigned char)(GfMul(a0, 9) ^ GfMul(a1, 14) ^ GfMul(a2, 11) ^ GfMul(a3, 13));
            state[2][c] = (unsigned char)(GfMul(a0, 13) ^ GfMul(a1, 9) ^ GfMul(a2, 14) ^ GfMul(a3, 11));
            state[3][c] = (unsigned char)(GfMul(a0, 11) ^ GfMul(a1, 13) ^ GfMul(a2, 9) ^ GfMul(a3, 14));
        }
    }

    // 最后一轮
    for (int r = 1; r < 4; ++r)
    {
        unsigned char tmp[8];
        int s = sh[r - 1];
        for (int c = 0; c < nb; ++c)
            tmp[c] = state[r][(c - s + nb) % nb];
        for (int c = 0; c < nb; ++c)
            state[r][c] = tmp[c];
    }
    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = kSBoxInv[state[r][c]];
    InvAddRoundKey(state, 0);

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            out[r + c * 4] = state[r][c];
