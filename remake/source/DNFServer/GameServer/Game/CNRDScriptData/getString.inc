const char* CNRDScriptData::getString(const char* key) const
{
    const std::string* p = m_strMap.get(key);
    if (p == 0)                 // b28c-L10：同族：命中分支跳走、默认串走 fall-through
        return "CNRDScriptData::Error";
    return p->c_str();
