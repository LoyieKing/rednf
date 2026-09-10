int STScriptFileList::GetIndex(const char* path)
{
    for (std::map<int, std::string>::iterator it = m_fileMap.begin();
         it != m_fileMap.end(); it++)
    {
        if ((*it).second == path)   // ORIG 用 operator*（de），非 operator->（pt）
            return (*it).first;
    }
    return -1;
