int STScriptFileList::FindIndexByFullScan(const char* path) const
{
    std::map<int, std::string>::const_iterator it = m_fileMap.begin();
    std::map<int, std::string>::const_iterator e = m_fileMap.end();
    while (it != e)
    {
        if (it->second == path)
            return it->first;
        it++;
    }
    return -1;
