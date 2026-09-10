const char* STScriptFileList::GetFileName(int idx) const
{
    std::map<int, std::string>::const_iterator it = m_fileMap.find(idx);
    if (it == m_fileMap.end())
        return 0;
    return it->second.c_str();
