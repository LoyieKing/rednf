bool WindowsDataFileFamily::LoadWindowsDataFile(const char* path,
                                                const char* name)   // 0x0857513c
{
    WindowsDataFile* f = new WindowsDataFile;
    if (m_files.empty())
    {
        StrCpyMaxPath(m_name, name);
    }
    if (!f->LoadWindowsDataFile(path))
    {
        delete f;
        return false;
    }
    // b28c-L19：ORIG 0x0857513c 重复内容分支为早返（delete 块落在 push 之前，
    // 尾部 return 1 共享），if/else 形会被 4.4 把 delete 挪到末尾 → DIFF。
    if (!IsDifferentFileData(f))
    {
        delete f;                                            // 内容重复仍返回 1
        return true;
    }
    if (m_maxSize < f->m_size)
    {
        m_maxSize = f->m_size;
    }
    m_files.push_back(f);
    return true;
}