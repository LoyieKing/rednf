stQuestShop::stQuestShop()
{
    // ORIG C1 0x83710d4：[map C1][initCost C1][map clear] 后 -1、memset(+0x28,0,0x4d)。
    m_map0.clear();
    m_field18 = -1;
    memset(m_pad28, 0, sizeof(m_pad28));
