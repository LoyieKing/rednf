void BestClearTime::_reset()
{
    // ORIG 0x80c8758：map.clear() + newRecord = false。
    m_map.clear();
    m_bNewRecord = false;
