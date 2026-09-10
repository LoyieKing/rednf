// ============================================================================
// GetWhiteAccount（ORIG T 0x084143ac）
// ============================================================================

bool DB_Login::GetWhiteAccount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect m_id from member_white_account where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        return true;
    }
    return false;
}
