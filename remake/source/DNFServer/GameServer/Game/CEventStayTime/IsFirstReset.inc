// ORIG 0x816bed4：真分支清零返回 1；假分支读回字段原值（movzbl 0x9）。
bool CEventStayTime::IsFirstReset()
{
    if (m_firstReset) {
        m_firstReset = false;
        return true;
    }
    return m_firstReset;
}