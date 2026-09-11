bool TaiwanInternalDispatcher::addDispatcher(TaiwanInternalPack::T code,
                                             TaiwanInterfaceInterDispatcher* disp)
{
    if (getDispatcher(code) != 0)
    {
        return false;
    }
    m_dispatchers[code] = disp;
    return true;
}