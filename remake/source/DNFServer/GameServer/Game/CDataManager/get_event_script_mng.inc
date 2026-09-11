void* CDataManager::get_event_script_mng()
{
    // ORIG 0x08110b62 W：return m_liveServerInfo.m_eventScriptMng（已类型化）
    return (void*)m_liveServerInfo.m_eventScriptMng;
}