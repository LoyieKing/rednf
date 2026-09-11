int STNameList::GetID(const char* name) const
{
    // ORIG const GetID 仍调非 const 的 StringUnorderedMap::get(char const*)；
    // 若直接 m_map.get(name) 会选 const 重载（_ZNK...），callee 不符，故去 const。
    const int* p =
        const_cast<StringUnorderedMap<CharStringHashTrait, int>&>(m_map).get(name);
    if (p == 0)
        return 0xffffffff;
    return *p;
}