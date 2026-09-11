bool CPacketTimeChecker::check()
{
    ++m_count;
    long now = time(0);
    // P59 极性：ORIG jge→后置赋值块，fall-through 为节流判断块。
    if (now - m_lastTime < validSecond_)
    {
        if (m_count > validCount_)
            return false;
    }
    else
    {
        m_lastTime = now;
        m_count = 0;
    }
    return true;
}