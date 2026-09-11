CBlowFish::CBlowFish(unsigned char* key, unsigned int keyLen, SBlock const& iv)
    : m_left(iv), m_right(iv)
{
    std::memset(m_pbox, 0, sizeof(m_pbox));
    if (keyLen != 0)
    {
        if (keyLen > 0x38)
            keyLen = 0x38;
        unsigned char localKey[0x38];
        std::memcpy(localKey, key, keyLen);
        std::memcpy(m_pbox, scm_auiInitP, sizeof(m_pbox));
        std::memcpy(m_sbox, scm_auiInitS, sizeof(m_sbox));

        unsigned char* kp = localKey;
        unsigned int j = 0;      // 密钥字节计数（遇 keyLen 回绕）
        for (unsigned int i = 0; i <= 0x11; ++i)
        {
            unsigned int word = 0;
            for (int n = 4; n != 0; --n)
            {
                word = (word << 8) | *kp;
                ++kp;
                ++j;
                if (j == keyLen)
                {
                    j = 0;
                    kp = localKey;
                }
            }
            m_pbox[i] ^= word;
        }

        SBlock block(0, 0);
        for (unsigned int i = 0; i <= 0x11; i += 2)
        {
            Encrypt(block);
            m_pbox[i] = block.m_data0;
            m_pbox[i + 1] = block.m_data1;
        }
        for (unsigned int r = 0; r <= 3; ++r)
        {
            for (unsigned int c = 0; c <= 0xff; c += 2)
            {
                Encrypt(block);
                m_sbox[r][c] = block.m_data0;
                m_sbox[r][c + 1] = block.m_data1;
            }
        }
    }
}