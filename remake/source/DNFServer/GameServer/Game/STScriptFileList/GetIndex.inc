// ORIG T 0x088badba，mangled _ZN16STScriptFileList8GetIndexEPKc：
//   按文件名遍历 map（与 FindIndexByFullScan 不同：非 const 迭代器、end()
//   每次循环调用、后缀 it++），命中返回索引，未命中返回 -1。
//   （证据见 docs/class_func_reports/STScriptFileList/GetIndex.md）
int STScriptFileList::GetIndex(const char* path)
{
    for (std::map<int, std::string>::iterator it = m_fileMap.begin();
         it != m_fileMap.end(); it++)
    {
        if ((*it).second == path)   // ORIG 用 operator*（de），非 operator->（pt）
            return (*it).first;
    }
    return -1;
}