STExpertJobScript::stDollControllerResult::stDollControllerResult()
{
    ZERO_WORD(&m_field8);                       // +8 word = 0
    m_field6 = m_field8;                        // +6 word = w+8
    m_field4 = m_field6;                        // +4 word = w+6
    m_field0 = static_cast<int>(
        static_cast<unsigned short>(m_field4)); // +0 dword = zext(w+4)
