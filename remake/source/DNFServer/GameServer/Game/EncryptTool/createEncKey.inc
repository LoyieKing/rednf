void EncryptTool::createEncKey()
{
    unsigned int local_10 = time(0);
    unsigned int local_14 = local_10;
    m_keyIndex = local_14 & 0xf;
    local_14 = m_keyIndex;
    Encryption::SeedRand(local_14);

    m_key[m_keyIndex][0]  = (char)(m_key[0][9]  + m_keyIndex + 0x05);
    m_key[m_keyIndex][1]  = (char)(m_key[1][8]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][2]  = (char)(m_key[2][7]  + m_keyIndex + 0x04);
    m_key[m_keyIndex][3]  = (char)(m_key[3][6]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][4]  = (char)(m_key[4][5]  + m_keyIndex);
    m_key[m_keyIndex][5]  = (char)(m_key[5][4]  + m_keyIndex + 0x07);
    m_key[m_keyIndex][6]  = (char)(m_key[6][3]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][7]  = (char)(m_key[7][2]  + m_keyIndex + 0x06);
    m_key[m_keyIndex][8]  = (char)(m_key[8][1]  + m_keyIndex + 0x03);
    m_key[m_keyIndex][9]  = (char)(m_key[9][0]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][10] = (char)(m_key[10][5] + m_keyIndex + 0x02);
    m_key[m_keyIndex][11] = (char)(m_key[11][4] + m_keyIndex);
    m_key[m_keyIndex][12] = (char)(m_key[12][3] + m_keyIndex + 0x07);
    m_key[m_keyIndex][13] = (char)(m_key[13][2] + m_keyIndex + 0x02);
    m_key[m_keyIndex][14] = (char)(m_key[14][1] + m_keyIndex + 0x06);
    m_key[m_keyIndex][15] = (char)(m_key[15][0] + m_keyIndex + 0x03);

    m_rijndael->Initialize((const char*)m_key[m_keyIndex], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
