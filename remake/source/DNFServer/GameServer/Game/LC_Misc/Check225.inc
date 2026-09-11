// ORIG T 0x0827b100 sz 0x187（Check225.md）：击杀间隔 2000ms 判定，上报 0xe1。
unsigned char Secu_MonsterCheck::Check225()
{
    if (m_field1c == m_curKillCount)
        return 1;
    char result = 1;
    // CUser.h 已落 bool；bare `if(field)` 复现 ORIG 8 位 test。
    if (m_active225)
    {
        int dt = m_setTick - m_field20;
        if (dt <= 1999)
        {
            m_field24 += 1;
            // 同 Check224：ORIG v=1 块在 fall-through，v=3 在 else-if。
            if (m_field24 > 4 && m_field24 <= 0xa)
            {
                m_field26 = 1;
            }
            else if (m_field24 > 0xa)
            {
                m_field26 = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                        3, 0, 0);
                }
                result = 0;
            }
        }
        else
        {
            if (m_field26 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                    1, 0, 0);
            }
            m_field24 = 0;
            m_field26 = 0;
        }
        m_field1c = m_curKillCount;
        m_field20 = m_setTick;
    }
    return result;
}