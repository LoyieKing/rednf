WindowsDataFile::~WindowsDataFile()
{                                                            // 0x08574e56
    if (m_data != 0)
    {
        delete m_data;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_data = 0;
    }
