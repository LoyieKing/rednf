void CNPCScriptList::_destroy()
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        it(m_npcMap.begin());
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        e(m_npcMap.end());
    while (it != e)
    {
        delete it->second;
        ++it;
    }
    m_npcMap.clear();
