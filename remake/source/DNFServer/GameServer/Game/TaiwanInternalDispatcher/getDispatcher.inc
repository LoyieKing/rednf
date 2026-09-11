TaiwanInterfaceInterDispatcher* TaiwanInternalDispatcher::getDispatcher(TaiwanInternalPack::T code)
{
    std::map<TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>::iterator it =
        m_dispatchers.find(code);
    if (it != m_dispatchers.end())
    {
        return it->second;
    }
    return 0;
}