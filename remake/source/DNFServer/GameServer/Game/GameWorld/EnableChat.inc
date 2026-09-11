bool GameWorld::EnableChat(const char* name)
{
    // ORIG 0x086cdc60：operator[] 逐访问点重调 4 次（P157），ban==0→already 后置块
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);   // ORIG 4.4 一行式会先求值 end()；两语句保 find→end 序
    if (it == m_idMap.end())
    {
        return false;
    }
    // bool 字段 `== 1` 探针实证生成 movzbl;test;je（xor-free），与 ORIG @086cdd53 一致
    if (m_idMap[str].m_chatBan == 1)
    {
        m_idMap[str].m_chatBan = false;
        m_idMap[str].m_chatEndTick = 0;
        // b28d-lane11：P221/P227 全表达式临时链（槽组翻正 + c_str 实参跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0x156c, 0)("%s is able to chat now\n", name);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1571, 0)("%s is already able to chat!\n", str.c_str());
    return true;
}