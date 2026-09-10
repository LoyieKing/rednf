unsigned int GameWorld::GetLeftTimeOfForbiddenMove(const char* name)
{
    // ORIG 0x086ce958：同 Chat 版但无 trace（strlen/find==end 直返 0）
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
    long long end = 0;
    end = G_GameWorld()->WhenIsTheEndOfForbiddenMove(name);
    if (end > 0)
    {
        unsigned int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        end -= cur1000;
        if (end > 0)
        {
            if (end < 60000)
            {
                return 60000;
            }
            return (unsigned int)end;
        }
        return 0;
    }
    return 0;
