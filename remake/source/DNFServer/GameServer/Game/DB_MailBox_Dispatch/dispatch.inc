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
