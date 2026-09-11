// ============================================================================
// DB_UpdateChuseokPackagePurchase::dispatch @ 0x0843c7bc
// mangled _ZN31DB_UpdateChuseokPackagePurchase8dispatchEiiP6Stream
// ============================================================================
bool DB_UpdateChuseokPackagePurchase::dispatch(int /*userIdx*/, int /*slot*/,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    *s >> mId;

    // ORIG：NumberToString(local_14, 0) 后直接拼入硬编码表名 SQL。
    db->set_query(
        "upDate event_1009_package_count set purchase_cnt = purchase_cnt+1 "
        "where m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true))
        return false;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1009_package_count(m_id, purchase_cnt) "
            "values(%s, %u)",
            NumberToString(mId, 0), 1);
        if (!db->exec(true))
            return false;
    }
    return true;
}
