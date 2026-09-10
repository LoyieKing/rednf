bool CEventScriptMng::is_event_entity(int key) const
{
    if (m_entities.find(key) != m_entities.end())
        return true;
    return false;
