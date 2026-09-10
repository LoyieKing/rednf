int CEventScriptMng::get_record_type(int key)
{
    // ORIG 0x810c7b8：独立 map find（不调 get_event_entity），
    // 未命中返回 0，命中虚调 get_event_record_type（vtable+0x14）。
    std::map<int, CEventEntity*>::iterator it = m_entities.find(key);
    if (it != m_entities.end())
        return it->second->get_event_record_type();
    return 0;
