// ORIG T 0x0827af78 sz 0x187（Check224.md）：击杀间隔 1000ms 判定，上报 0xe0。
unsigned char Secu_MonsterCheck::Check224()
{
    if (m_field12 == m_curKillCount)
        return 1;
    char result = 1;
    // CUser.h 已落 bool；bare `if(field)` 复现 ORIG `movzbl;test %al`（8 位，
    // `!=0` 对 bool 会生 movzbl+movzbl+testl 异形）。
    if (m_active224)
    {
        int dt = m_setTick - m_field14;
        if (dt <= 0x3e8)
        {
            m_field18 += 1;
            // ORIG 块排布：两 cmp 直跳 else（P133 合并短路 fall-through =
            // v=1 块），v=3 在 else-if；反向嵌套 if 形会把 v=1 块挤到尾部。
            if (m_field18 > 4 && m_field18 <= 0xa)
            {
                m_field1a = 1;
            }
            else if (m_field18 > 0xa)
            {
                m_field1a = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                        3, 0, 0);
                }
                result = 0;
            }
        }
        else
        {
            if (m_field1a == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                    1, 0, 0);
            }
            m_field18 = 0;
            m_field1a = 0;
        }
        m_field12 = m_curKillCount;
        m_field14 = m_setTick;
    }
    return result;
}