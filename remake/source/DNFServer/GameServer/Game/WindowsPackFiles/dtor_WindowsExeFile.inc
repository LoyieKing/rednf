WindowsExeFile::~WindowsExeFile()
{                                                            // 0x0857371a
    if (m_image != 0)
    {
        delete m_image;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_image = 0;
    }
    // ORIG 0x08573744..：迭代器形——独立 C1Ev（0x0857374e）+ begin 临时槽
    // （-0x1c）再拷入 it（-0x14）；每轮 end() 重取临时 + operator!=/operator*，
    // delete 后 *it=0，operator++ 回边；无 size/operator[] 索引链（P50 复活）。
    std::vector<WindowsExeConstantSection*>::iterator it;
    it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_sections.clear();
    it = m_commonSections.begin();
    for (; it != m_commonSections.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_commonSections.clear();
}