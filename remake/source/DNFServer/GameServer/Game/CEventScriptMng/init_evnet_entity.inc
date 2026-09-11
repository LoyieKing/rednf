void CEventScriptMng::init_evnet_entity()
{
    init_event_nums();
    for (int i = 0; (int)m_eventRecordTypes.size() > i; ++i)
    {
        if (get_event_entity(m_eventRecordTypes[i]) == 0)
            m_entities[m_eventRecordTypes[i]] =
                create_event_entity(m_eventRecordTypes[i], true);
    }
}