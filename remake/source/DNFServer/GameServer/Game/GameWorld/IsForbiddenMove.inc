bool GameWorld::IsForbiddenMove(const char* name)
{
    
    // ORIG 0x086cdb60：同 IsForbiddenChat 形态，字段 m_moveBan(+0x05)
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_moveBan != 0)
    {
        if (GetLeftTimeOfForbiddenMove(name) == 0)
        {
            EnableMove(name);
            return false;
        }
        // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x1557, 0)(
            "%s is forbidden to move\n", name);
        return true;
    }
    return false;
