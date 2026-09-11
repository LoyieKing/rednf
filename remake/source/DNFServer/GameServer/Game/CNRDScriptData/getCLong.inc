int CNRDScriptData::getCLong(const char* key) const
{
    const int* p = m_clongMap.get(key);
    if (p == 0)                 // b28c-L10：同 getInt，ORIG 0x…getCLongEPKc 同型
        return -1;
    return *p;
}