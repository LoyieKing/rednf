bool DB_SecurityCardUpdateCancelCnt::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();

    db->set_query(
        "upDate member_security_card set cancel_cnt=%d where m_id=%s and apply_flag=1",
        reinterpret_cast<SecurityCardCancelCntView*>(sig)->m_cancelCnt,
        NumberToString(
            reinterpret_cast<SecurityCardCancelCntView*>(sig)->m_mId, 0));
    db->exec(true);
    return 1;
