bool CEventScriptMng::is_current_ingame_event_record_type(int key)
{
    std::map<int, InGameEventInfo>::iterator it = m_info.find(key);
    if (it != m_info.end())
        return true;
    return false;
