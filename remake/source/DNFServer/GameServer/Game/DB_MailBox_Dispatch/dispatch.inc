// ============================================================================
// 2. DB_MailBox_Req_Delete::dispatch —— ORIG 0x0841f084（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_DEL_MAIL> → GetDBHandle(DB_HANDLE_3,0)；
//    count==0 返 0；count==1 单条 upDate（postal_id=%d）；否则
//    getList2inQuery 生成列表串（postal_id %s）；exec(true) 失败返 0，成功返 1。
//    局部 static char strQuery[]（ORIG _ZZN21DB_MailBox_Req_Delete8dispatch
//    EiiP6StreamE8strQuery .bss 证据）。
// ============================================================================
bool DB_MailBox_Req_Delete::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_DEL_MAIL* sig = stream->GetOutBuffer<SIG_MAILBOX_DEL_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    // 具名字段绑定（0x0841f084 报告：+0x00 count movl、+0x04 首 idx、+0x54 SQL
    // delete_flag 实参；SIG_MAILBOX_DEL_MAIL union 视图见 SigTypes_D.h）。
    if (sig->m_count == 0)
        return 0;

    if (sig->m_count == 1)
    {
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id=%d",
            sig->m_type, sig->m_postalIds[0]);
    }
    else
    {
        // [R21scanPH §27.3] ORIG `_ZZN21DB_MailBox_Req_Delete8dispatchEiiP6StreamE8strQuery`
        // nm size = 4098 (0x1002) @0x943c240（.bss），下一符号
        // `_ZZ24put_compressed_blob_dataP5MySQLiPciPS1_E15compressedLength` @0x943d244 闭合（+2）
        // ⇒ 原码缓冲为 4098 字节；原 0x100 小于 ORIG ⇒ 复刻 ORIG 的截断/越界语义（getList2inQuery
        // 用 sprintf 无界追加，缓冲尺寸即行为边界）。
        static char strQuery[0x1002];
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id %s",
            sig->m_type, getList2inQuery(&sig->m_count, strQuery));
    }

    if (!db->exec(true))
        return 0;
    return 1;
}