CNPCScript* CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it =
        m_npcMap.find(idx);
    if (it == m_npcMap.end())
        return 0;
    return it->second;
