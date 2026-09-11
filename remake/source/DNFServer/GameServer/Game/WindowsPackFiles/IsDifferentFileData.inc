bool WindowsDataFileFamily::IsDifferentFileData(WindowsDataFile* f)    // 0x08575068（非 const）
{
    // b28c-L19：ORIG 0x08575068 迭代器先默认构造（C1Ev @0x8576bb6）再经 begin()
    // 临时槽（-0x1c）拷贝赋值到 it（-0x14）——"先声明后赋值"形（P28/P50 反例，
    // ORIG 确有 C1Ev 且 begin()/de() 均为 const 版 `_ZNK`）；memcmp 长度照抄 f->m_size。
    std::vector<WindowsDataFile*>::iterator it;
    it = m_files.begin();
    for (; it != m_files.end(); ++it)
    {
        WindowsDataFile* cur = *it;
        if (cur->m_size == f->m_size &&
            memcmp(cur->m_data, f->m_data, f->m_size) == 0)
        {
            return false;                                    // 相同内容 → 非新文件
        }
    }
    return true;
}