WindowsDataFileFamily::~WindowsDataFileFamily()
{                                                            // 0x08574f7c
    // [b28d-64] P50 复活：ORIG 0x08574f8a 独立 iterator::C1Ev + begin() 临时槽
    // （-0x1c）再拷入 it（-0x10）⇒ 先声明后赋值；索引 for 会少默认构造与临时槽。
    std::vector<WindowsDataFile*>::iterator it;
    it = m_files.begin();
    for (; it != m_files.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_files.clear();
