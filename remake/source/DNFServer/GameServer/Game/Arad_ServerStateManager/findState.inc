bool Arad_ServerStateManager::findState(ENUM_GLOBAL_EFFECT_ARAD code,
                                        AradServerStateMessage& out)
{
    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>::iterator it =
        m_stateMap.find(code);
    if (it != m_stateMap.end())
    {
        out = it->second;
        return true;
    }
    memset(&out, 0, sizeof(out));
    return false;
