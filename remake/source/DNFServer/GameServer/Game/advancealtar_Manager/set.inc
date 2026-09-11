// ---- ORIG T 0x08131616（CharacAdvanceAltarManager/set.md）：单实例分派，
//      db.m_db(+4) → m_data.set。----
void CharacAdvanceAltarManager::set(const _CharacAdvanceAltarDbData& db)
{
    for (int i = 0; i < 1; ++i)
    {
        m_data[i].set(db.m_db[i]);
    }
}