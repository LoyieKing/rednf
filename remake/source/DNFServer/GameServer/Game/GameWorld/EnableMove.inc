bool GameWorld::EnableMove(const char* name)
{
    // ORIG 0x086cde60：同 EnableChat 形态，字段 m_moveBan(+0x05)/m_moveEndTick(+0x0c)
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
    if (m_idMap[str].m_moveBan == 1)
    {
        m_idMap[str].m_moveBan = false;
        m_idMap[str].m_moveEndTick = 0;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x1584, 0)("%s is able to move now\n", name);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1589, 0)("%s is already able to move!\n", str.c_str());
    return true;
}