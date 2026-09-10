void CharacAdvanceAltarManager::getCharacAdvanceAltarDbData(_CharacAdvanceAltarDbData& out) const
{
    // ORIG 循环 local_10 <= 0（一次），m_data 按 stride 0xd0 取第 i 行
    for (int local_10 = 0; local_10 <= 0; ++local_10)
    {
        m_data[local_10].get(out.m_db[local_10]);
    }
