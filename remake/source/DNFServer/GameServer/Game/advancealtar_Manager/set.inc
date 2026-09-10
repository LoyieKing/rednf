void CharacAdvanceAltarManager::set(const _CharacAdvanceAltarDbData& db)
{
    for (int i = 0; i < 1; ++i)
    {
        m_data[i].set(db.m_db[i]);
    }
