bool WindowsExeFileFamily::IsDifferentFileData(WindowsExeFile* f) // 0x08574ace
{
    // [b28c-L29] ORIG 0x08574ace：迭代器先默认构造（C1Ev 调用 @0x85766a6）再经
    // begin() 临时槽（-0x1c）拷贝赋值到 it（-0x14）——"先声明后赋值"形（P50 反例，
    // ORIG 确有 C1Ev 符号）；每轮 end/operator!=/operator* 直调。
    std::vector<WindowsExeFile*>::iterator it;
    it = m_exeFiles.begin();
    for (; it != m_exeFiles.end(); ++it)
    {
        WindowsExeFile* cur = *it;
        if (cur->m_size == f->m_size &&
            memcmp(cur->m_image, f->m_image, f->m_size) == 0)
        {
            return false;
        }
    }
    return true;
}