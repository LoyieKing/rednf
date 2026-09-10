void CEventScriptMng::get_ingame_event_record_type_list(std::vector<int>& out)
{
    out.clear();
    for (std::map<int, InGameEventInfo>::iterator it = m_info.begin(),
             endIt = m_info.end();
         it != endIt; ++it)
        out.push_back(it->first);
