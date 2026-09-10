// ============================================================================
// _getPassPadInfo（ORIG T 0x0841144a）
// ============================================================================

int DB_Login::_getPassPadInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    sig->m_passPadEnable = 0;
    sig->m_passPadCancelCnt = 0;                    // +0x3918
    sig->m_passPadFailCnt = 0;                      // +0x3914
    memset(sig->m_passPadPassword, 0, 0x24);        // +0x38e1（仅清 0x24 缓冲）
    db->set_query("seLect mousepass,fail_cnt,cancel_cnt from member_mousepass where m_id=%s and enable_flag = '1'",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, sig->m_passPadPassword, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, sig->m_passPadFailCnt))
    {
        return 1;
    }
    if (!db->get_int(2, sig->m_passPadCancelCnt))
    {
        return 1;
    }
    sig->m_passPadEnable = 1;
    if (sig->m_passPadFailCnt > 2)
    {
        return 0x49;    // fail_cnt>=3 触发二次认证错误码
    }
    return 0;
}
