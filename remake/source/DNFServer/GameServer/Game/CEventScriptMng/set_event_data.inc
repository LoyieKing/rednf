void CEventEntity::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810b03a：+0x18=key，+0x14=script。
    m_recordType = key;
    m_script = script;
}