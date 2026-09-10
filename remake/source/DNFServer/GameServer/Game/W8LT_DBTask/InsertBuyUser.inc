void DB_BillingRecord::InsertBuyUser(SIG_BILLING_RECORD* sig, bool& result)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE);
    result = false;
    db->set_query(
        " upDate prod_buy_user  set user_id = '%s', sex = %d, birthday = '%s', last_buy_time = now()  where m_id = %d ",
        ((DB_BillingRecordSigView*)sig)->m_userId, ((DB_BillingRecordSigView*)sig)->m_sex,
        ((DB_BillingRecordSigView*)sig)->m_birthday, ((DB_BillingRecordSigView*)sig)->m_mId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6117,
                              "[QUERY_ERROR] (upDate prod_buy_user m_id:%d)",
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        return;
    }
    db->set_query("seLect m_id, user_id from prod_buy_user where m_id = %d",
                  ((DB_BillingRecordSigView*)sig)->m_mId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6120,
                              "[QUERY_ERROR] (seLect prod_buy_user m_id:%d)",
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        return;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query(
            " inSert into prod_buy_user ( m_id, user_id, sex, birthday, first_buy_time, last_buy_time )  values(%d, '%s', %d, '%s', now(), now() ) ",
            ((DB_BillingRecordSigView*)sig)->m_mId, ((DB_BillingRecordSigView*)sig)->m_userId,
            ((DB_BillingRecordSigView*)sig)->m_sex, ((DB_BillingRecordSigView*)sig)->m_birthday);
        result = true;
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x6134,
                                  "[QUERY_ERROR] (inSert into prod_buy_user m_id:%d)",
                                  ((DB_BillingRecordSigView*)sig)->m_mId);
        }
    }
