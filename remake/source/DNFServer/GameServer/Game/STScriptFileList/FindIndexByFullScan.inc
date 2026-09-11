//   ORIG 循环形态：begin() 一次、end() 缓存于循环外（P13）、后缀 it++（ppEi）。
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
}