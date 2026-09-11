// ORIG 0x810b9d4（const）：map find != end。
bool CEventScriptMng::is_event_entity(int key) const
{
    if (m_entities.find(key) != m_entities.end())
        return true;
    return false;
}