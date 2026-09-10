void CUserPremium::RemovePremium(int type)
{
    // ORIG 0x86adba6：计数递减走 std::max<int>(zero, dec) 调用形（两临时
    // 槽分支各独立，非三目）；链全重算。
    // P309：声明序 = ORIG 槽浅→深（勿改序，否则槽位回退）。
    int zero2;
    int dec2;
    int zero1;
    int dec1;
    if (m_entries[type].m_state != 2)
    {
        m_entries[type].m_type = 0;
        if (g_SPremiumInfo[type].m_kind == 1)
        {
            dec1 = m_normalCount + -1;
            zero1 = 0;
            m_normalCount = std::max(zero1, dec1);
            return;
        }
        dec2 = m_restrictedCount + -1;
        zero2 = 0;
        m_restrictedCount = std::max(zero2, dec2);
        return;
    }
    return;
