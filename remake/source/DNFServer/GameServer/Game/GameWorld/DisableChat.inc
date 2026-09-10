bool GameWorld::DisableChat(const char* name, int secs)
{
    // ORIG 0x086ce054：ban==1 检查（xor $0x1 物化，P158）；already 块后置；写侧 2 次独立 operator[]
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
    // bool 字段 `== 0`/`!x` 探针实证生成 movzbl;xor $1;test;je，与 ORIG @086ce147 一致
    if (m_idMap[str].m_chatBan == 0)
    {
        m_idMap[str].m_chatBan = true;
        int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        m_idMap[str].m_chatEndTick = secs * 1000 + cur1000;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15a2, 0)("%s is not able to chat now for %d secs\n", name, secs);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x15a7, 0)("%s is already not able to chat!\n", str.c_str());
    return true;
