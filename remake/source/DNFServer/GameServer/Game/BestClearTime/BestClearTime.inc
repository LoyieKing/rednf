BestClearTime::BestClearTime()
{
    // ORIG 0x80c851c：基类 CData 构造后，map 默认构造并立即 clear，
    // newRecord 清零。
    m_map.clear();
    m_bNewRecord = false;
