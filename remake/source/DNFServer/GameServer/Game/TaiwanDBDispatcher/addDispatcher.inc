bool TaiwanDBDispatcher::addDispatcher(TaiwanInternalPack::T code,
                                       TaiwanInterfaceDBDispatcher* disp)
{
    if (getDispatcher(code) != 0)
    {
        return false;
    }
    m_dispatchers[code] = disp;
    return true;
}
