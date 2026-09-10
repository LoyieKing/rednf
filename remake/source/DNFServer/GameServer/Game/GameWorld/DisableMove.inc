bool GameWorld::DisableMove(const char* name, int secs)
{
    // ORIG 0x086ce280：同 DisableChat 形态，字段 m_moveBan/m_moveEndTick
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
    if (m_idMap[str].m_moveBan == 0)
    {
        m_idMap[str].m_moveBan = true;
        int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        m_idMap[str].m_moveEndTick = secs * 1000 + cur1000;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15c0, 0)("%s is not able to move now for %d secs\n", name, secs);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x15c5, 0)("%s is already not able to move!\n", str.c_str());
    return true;
