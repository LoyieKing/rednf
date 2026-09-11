float CNRDScriptData::getFloat(const char* key) const
{
    const float* p = m_floatMap.get(key);
    if (p == 0)                 // b28c-L10：同上，ORIG 0x…getFloatEPKc 同型
        return -1.0f;
    return *p;
}