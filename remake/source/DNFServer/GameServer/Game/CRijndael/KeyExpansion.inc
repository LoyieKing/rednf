void CRijndael::KeyExpansion()
{
    int nk = m_keyLength / 4;
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    int total = nb * (nr + 1);

    for (int i = 0; i < nk; ++i)
    {
        m_w[i] = ((unsigned int)m_key[i * 4] << 24) |
                 ((unsigned int)m_key[i * 4 + 1] << 16) |
                 ((unsigned int)m_key[i * 4 + 2] << 8) |
                 (unsigned int)m_key[i * 4 + 3];
    }

    for (int i = nk; i < total; ++i)
    {
        unsigned int temp = m_w[i - 1];
        if (i % nk == 0)
        {
            temp = SubWord(RotWord(temp)) ^ (unsigned int)kRcon[i / nk - 1] << 24;
        }
        else if (nk > 6 && i % nk == 4)
        {
            temp = SubWord(temp);
        }
        m_w[i] = m_w[i - nk] ^ temp;
    }
