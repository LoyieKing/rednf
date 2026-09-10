stHacktypeExceptionInfo_t CAutoPunishRuleHackTypeMgr::GetExceptionCnt(int key)
{
    std::map<int, stHacktypeExceptionInfo_t>::iterator it = m_exceptionMap.find(key);
    if (it != m_exceptionMap.end())
    {
        return it->second;
    }
    return stHacktypeExceptionInfo_t(0, 0);
