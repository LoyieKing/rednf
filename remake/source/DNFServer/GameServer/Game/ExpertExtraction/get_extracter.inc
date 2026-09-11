// ============================================================================
// CExtractionMgr::get_extracter —— ORIG T 0x084a3166（报告 get_extracter.md）
// ============================================================================
CExtraction* CExtractionMgr::get_extracter(unsigned int index)
{
    if (index > 2)
    {
        return 0;
    }
    return m_slots[index];
}