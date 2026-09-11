void CRijndael::AddRoundKey(unsigned char state[4][8], int round)
{
    int nb = m_blockSize / 4;
    for (int c = 0; c < nb; ++c)
    {
        unsigned int w = m_w[round * nb + c];
        state[0][c] = (unsigned char)(state[0][c] ^ ((w >> 24) & 0xff));
        state[1][c] = (unsigned char)(state[1][c] ^ ((w >> 16) & 0xff));
        state[2][c] = (unsigned char)(state[2][c] ^ ((w >> 8) & 0xff));
        state[3][c] = (unsigned char)(state[3][c] ^ (w & 0xff));
    }
}