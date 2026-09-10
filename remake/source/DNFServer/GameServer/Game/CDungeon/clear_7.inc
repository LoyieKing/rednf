void STWarRoomData::clear()
{
    // ORIG 0x836a398：两个 vector<int>[4] 数组循环 + 标量区。
    memset(&m_0, 0, 0xc);
    m_c = 0;
    m_10 = -1;
    for (int i = 0; i < 4; ++i)
        m_vec14[i].clear();
    m_vec44.clear();
    m_50 = 0;
    m_54 = 0;
    m_58 = 0;
    memset(m_5c, 0, sizeof(m_5c));
    for (int i = 0; i < 4; ++i)
        m_vec6c[i].clear();
    memset(m_9c, 0, sizeof(m_9c));
    m_ac = 0;
    m_spawnProb.clear();
    m_bc = 0;
    m_c0 = 0;
    memset(m_c4, 0, sizeof(m_c4));
    m_d4 = 0;
    memset(m_dc, 0, sizeof(m_dc));
    m_d8 = 0;
    m_ec = 0;
    m_f0 = 0;
    m_f4 = 0;
    m_f8 = 0;
    m_fc = 0;
