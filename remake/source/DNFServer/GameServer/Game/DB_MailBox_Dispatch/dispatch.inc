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
        static char strQuery[0x100];
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id %s",
            sig->m_type, getList2inQuery(&sig->m_count, strQuery));
    }

    if (!db->exec(true))
        return 0;
    return 1;
}