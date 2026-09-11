// ----------------------------------------------------------------------------
// GetExceptionCnt —— ORIG T 0x080f972a (0x8a)，sret 返回 POD(4B)：
// find(key)；命中取 pair.second，未命中 stHacktypeExceptionInfo_t(0,0)。
// ----------------------------------------------------------------------------
stHacktypeExceptionInfo_t CAutoPunishRuleHackTypeMgr::GetExceptionCnt(int key)
{
    std::map<int, stHacktypeExceptionInfo_t>::iterator it = m_exceptionMap.find(key);
    if (it != m_exceptionMap.end())
    {
        return it->second;
    }
    return stHacktypeExceptionInfo_t(0, 0);
}