bool WindowsExeFileFamily::LoadWindowsExeFile(const char* path,
                                              const char* name)     // 0x0857463e
{
    WindowsExeFile* exe = new WindowsExeFile;
    if (m_exeFiles.empty())
    {
        StrCpyMaxPath(m_name, name);
    }
    if (!exe->LoadWindowsExeFile(path))
    {
        delete exe;
        return false;
    }
    // ORIG 0x085746d5：`!IsDifferentFileData(exe)`（xor$1 取反）命中 → 先
    // delete 重复项并 return 1（重复内容视为成功），push 分支在其后。
    if (!IsDifferentFileData(exe))
    {
        delete exe;
        return true;
    }
    m_exeFiles.push_back(exe);
    return true;
