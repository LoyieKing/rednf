expert_extraction::CExtractionMgr::~CExtractionMgr()
{
    // ORIG 0x84a311a：for i∈[0,2] if(m_slots[i]) delete m_slots[i];（不置空）。
    for (int i = 0; i < 3; ++i)
    {
        delete m_slots[i];   // 4.4 delete 自带 test/je 判空（P207）→ 与 ORIG 单判同形
    }
}