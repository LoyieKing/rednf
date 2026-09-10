float CNRDScriptData::getCFloat(const char* key) const
{
    const float* p = m_cfloatMap.get(key);
    if (p == 0)                 // b28c-L10：同族（台账极性族含此条）
        return -1.0f;
    return *p;
