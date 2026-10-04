// ORIG 0x8443c62：GetOutBuffer<SIG_MILEAGE_STRUCT> → GetDBHandle(1,0) 失败
// cMyTrace(0xa79a) 返 0 → upDate member_miles set miles=%d(+8) where m_id=%u
// (+0)；返回 exec(true)。
bool DB_SaveMileage::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIndex = 1;  // ORIG _ZZN14DB_SaveMileage8dispatch…E11handleIndex
    SIG_MILEAGE_STRUCT* sig = stream->GetOutBuffer<SIG_MILEAGE_STRUCT>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIndex, E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa79a, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 1);
        return 0;
    }
    db->set_query("upDate member_miles set miles = %d where m_id = %u",
                  sig->m_mileage, sig->m_accId);
    return db->exec(true);
}