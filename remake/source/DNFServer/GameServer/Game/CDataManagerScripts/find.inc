// ORIG T 0x08581680：hash_map 命中返回 CNPCScript*，未命中返回 0。
CNPCScript* CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it =
        m_npcMap.find(idx);
    if (it == m_npcMap.end())
        return 0;
    return it->second;
}