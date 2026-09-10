CEventScriptMng::CEventScriptMng()
    : InGameEventInfoList(), InGameEventScriptLoader(),
      m_entities(), m_todayTime(0), m_todayTimeDay(0), m_eventRecordTypes()
{
    m_entities.clear();
    m_info.clear();
    m_scripts.clear();
