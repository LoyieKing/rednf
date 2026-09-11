// ORIG 0x810b96a：map find，命中返回实体指针，未命中返回 0。
CEventEntity* CEventScriptMng::get_event_entity(int key)
{
    // ORIG 0x810b96a：find != end 先行（ne），命中返回 second。
    std::map<int, CEventEntity*>::iterator it = m_entities.find(key);
    if (it != m_entities.end())
        return it->second;
    return 0;
}