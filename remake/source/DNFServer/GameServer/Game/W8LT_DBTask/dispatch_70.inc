// ============================================================================
// DB_UpdateItemLock::dispatch —— ORIG 0x0843993a（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_ITEM_LOCK_UPDATE> → 压缩 sig+4 起 800 字节
// 至 buf（0x3cc 清 0）；compress 失败返 0 → upDate charac_item_lock_info
// （item_lock_info='%s'，charac_no=%u）exec 失败返 0；affected==0 补 inSert；
// inSert exec 失败返 0；全绿返 1。无基类 dispatch 前置检查。
bool DB_UpdateItemLock::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    SIG_ITEM_LOCK_UPDATE* sig =
        stream->GetOutBuffer<SIG_ITEM_LOCK_UPDATE>();
    char buf[0x3cc] = { 0 };
    unsigned int bufLen = 0x3cc;
    if (!compress_zip(buf, bufLen, (char*)sig->m_locks, 800))
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
}