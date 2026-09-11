// ============================================================================
// DB_DeleteAccountCargo::dispatch —— ORIG 0x0843b24e（报告 dispatch.md 全文）
// ============================================================================
// *stream >> m_id(uint) → GetDBHandle(DB_HANDLE_2,0) → set_query("deLete from
// account_cargo where m_id = %s", NumberToString(m_id,0)) → 返 exec(true)==1。
// 无回包、无日志。
// ============================================================================
bool DB_DeleteAccountCargo::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    unsigned int mId = 0;
    MySQL* db;
    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("deLete from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    return 1;
}