bool DB_UpdateItemLock::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateItemLockView* sig =
        (DB_UpdateItemLockView*)stream->GetOutBuffer<SIG_ITEM_LOCK_UPDATE>();
    char buf[0x3cc] = { 0 };
    unsigned int bufLen = 0x3cc;
    if (!compress_zip(buf, bufLen, sig->m_lockData, 800))
        return 0;
    {
        // register 局部直落 %ebx（ORIG +0x91 `mov (%eax),%ebx` 免中转拷贝）。
        register int characNo = sig->m_characNo;
        db->set_query(
            "upDate charac_item_lock_info set item_lock_info='%s' where charac_no=%u",
            db->blob_to_str(0, buf, (int)bufLen), characNo);
    }
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_lock_info(charac_no, item_lock_info) values(%u, '%s')",
            (unsigned int)sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
