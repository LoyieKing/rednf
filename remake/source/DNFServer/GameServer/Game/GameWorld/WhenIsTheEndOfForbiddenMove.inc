unsigned int GameWorld::WhenIsTheEndOfForbiddenMove(const char* name)
{
    // ORIG 0x086ce5f0：同 WhenIsTheEndOfForbiddenChat 形态，字段 m_moveBan/m_moveEndTick
    if (name == 0)
    {
        return 0;
    }
    if (strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return 0;
    }
    if (m_idMap[str].m_moveBan)
    {
        register int tick = m_idMap[str].m_moveEndTick;
        return tick;
    }
    return 0;
}