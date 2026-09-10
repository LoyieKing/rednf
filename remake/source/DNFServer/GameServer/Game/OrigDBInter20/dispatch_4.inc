bool DB_SecurityCardAddCertCnt::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_ADD_CERT_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>();

    db->set_query(
        "upDate member_security_card set cert_cnt = cert_cnt+1  where m_id=%s and apply_flag=1",
        NumberToString(
            reinterpret_cast<SecurityCardAddCertCntView*>(sig)->m_mId, 0));
    db->exec(true);
    return 1;
