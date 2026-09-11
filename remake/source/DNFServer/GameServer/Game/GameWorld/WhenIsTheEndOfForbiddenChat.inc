unsigned int GameWorld::WhenIsTheEndOfForbiddenChat(const char* name)
{
    // ORIG 0x086ce4a0：两次独立 operator[]（P157 逐字段重读，勿缓存引用）
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
    if (m_idMap[str].m_chatBan)
    {
        // ORIG @086ce590 mov 0x8(%eax),%ebx 直落 callee-saved：块内 register int
        // 与字段类型（int +0x08）精确一致（P-技巧）
        register int tick = m_idMap[str].m_chatEndTick;
        return tick;
    }
    return 0;
}