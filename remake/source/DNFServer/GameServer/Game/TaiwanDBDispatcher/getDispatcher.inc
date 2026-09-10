TaiwanInterfaceDBDispatcher* TaiwanDBDispatcher::getDispatcher(TaiwanInternalPack::T code)
{
    std::map<TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*>::iterator it =
        m_dispatchers.find(code);
    if (it != m_dispatchers.end())
    {
        return it->second;
    }
    return 0;
}
