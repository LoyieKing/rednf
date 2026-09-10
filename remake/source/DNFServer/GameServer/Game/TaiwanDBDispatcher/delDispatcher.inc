void TaiwanDBDispatcher::delDispatcher(TaiwanInternalPack::T code)
{
    if (getDispatcher(code) != 0)
    {
        m_dispatchers.erase(m_dispatchers.find(code));
    }
