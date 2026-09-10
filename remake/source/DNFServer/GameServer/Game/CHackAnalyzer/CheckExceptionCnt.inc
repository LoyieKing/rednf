bool CHackAnalyzer::CheckExceptionCnt(ENUM_HACKTYPE type, int cnt)
{
    // ORIG T 0x080f972a：GetExceptionCnt sret 调用，命中复制 {cnt,flag}，否则置零。
    stHacktypeExceptionInfo_t info =
        GlobalData::g_autoPunishRuleHackTypeMgr->GetExceptionCnt((int)type);

    if (info.m_cnt != 0)
    {
    // ORIG：res 先默认构造，再 insert 结果移动赋值；make_pair(type,1) 无 cast
    // （first 以 ENUM_HACKTYPE& 绑参数槽）。
        std::pair<std::map<int, int>::iterator, bool> res;
        res = m_exceptionCntMap.insert(std::make_pair(type, 1));
        if (!res.second)
        {
            res.first->second = res.first->second + cnt;
            if (res.first->second >= (int)info.m_cnt)
                return false;
        }
        return true;
    }
    if (info.m_flag != 0 && (int)info.m_flag >= cnt)
        return true;
    return false;
