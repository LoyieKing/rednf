CExtraction* CExtractionMgr::get_extracter(unsigned int index)
{
    if (index > 2)
    {
        return 0;
    }
    return m_slots[index];
