void DB_BillingRecord::InsertSaleEntry(SIG_BILLING_RECORD* sig, bool isNew)
{
    int buyType;
    struct tm tmVal;
    time_t t;
    char quarter[5] = { 0 };
    MySQL* db;
    time(&t);
    localtime_r(&t, &tmVal);
    sprintf(quarter, "%02d%d", (tmVal.tm_year + 0x76c) % 2000, tmVal.tm_mon / 3 + 1);
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE);
    if (isNew)
        buyType = 1;
    else
        buyType = 2;
    db->set_query(
        " insert into prod_sale_entry_%s  ( agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id  , server_id, charac_no, job, grow_type, level, prize_flag, new_flag, result_code, result_message, gift_server_info, gift_get_charac_no  , charge_price, free_price, count_no, each_price, price_type, user_ip, buy_type, tran_id )  values( '%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, '%s', %u, %u ) ",
        quarter,
        (char*)sig, ((DB_BillingRecordSigView*)sig)->m_sealFlag,
        ((DB_BillingRecordSigView*)sig)->m_saleTime, ((DB_BillingRecordSigView*)sig)->m_price,
        ((DB_BillingRecordSigView*)sig)->m_slotPartType,
        ((DB_BillingRecordSigView*)sig)->m_slotPartCode,
        ((DB_BillingRecordSigView*)sig)->m_ipgNo, ((DB_BillingRecordSigView*)sig)->m_mId,
        ((DB_BillingRecordSigView*)sig)->m_userId, ((DB_BillingRecordSigView*)sig)->m_serverId,
        ((DB_BillingRecordSigView*)sig)->m_characNo, ((DB_BillingRecordSigView*)sig)->m_job,
        ((DB_BillingRecordSigView*)sig)->m_growType, ((DB_BillingRecordSigView*)sig)->m_level,
        ((DB_BillingRecordSigView*)sig)->m_prizeFlag, buyType,
        ((DB_BillingRecordSigView*)sig)->m_resultCode,
        ((DB_BillingRecordSigView*)sig)->m_resultMessage,
        ((DB_BillingRecordSigView*)sig)->m_giftServerInfo,
        ((DB_BillingRecordSigView*)sig)->m_giftGetCharacNo,
        ((DB_BillingRecordSigView*)sig)->m_chargePrice,
        ((DB_BillingRecordSigView*)sig)->m_freePrice, ((DB_BillingRecordSigView*)sig)->m_countNo,
        ((DB_BillingRecordSigView*)sig)->m_eachPrice,
        ((DB_BillingRecordSigView*)sig)->m_priceType, ((DB_BillingRecordSigView*)sig)->m_userIp,
        ((DB_BillingRecordSigView*)sig)->m_buyType,
        ((DB_BillingRecordSigView*)sig)->m_tranNoLow,
        ((DB_BillingRecordSigView*)sig)->m_tranNoHigh);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6191,
                              "db insert error prod_sale_entry_%s m_id: %d", quarter,
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        if (isNew)
            buyType = 1;
        else
            buyType = 2;
        db->set_query(
            " insert into prod_sale_entry_073 ( agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id, server_id, charac_no, job, prize_flag, new_flag, result_code, result_message )  values( '%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, '%s' ) ",
            (char*)sig, ((DB_BillingRecordSigView*)sig)->m_sealFlag,
            ((DB_BillingRecordSigView*)sig)->m_saleTime,
            ((DB_BillingRecordSigView*)sig)->m_price,
            ((DB_BillingRecordSigView*)sig)->m_slotPartType,
            ((DB_BillingRecordSigView*)sig)->m_slotPartCode,
            ((DB_BillingRecordSigView*)sig)->m_ipgNo,
            ((DB_BillingRecordSigView*)sig)->m_mId,
            ((DB_BillingRecordSigView*)sig)->m_userId,
            ((DB_BillingRecordSigView*)sig)->m_serverId,
            ((DB_BillingRecordSigView*)sig)->m_characNo,
            ((DB_BillingRecordSigView*)sig)->m_job,
            ((DB_BillingRecordSigView*)sig)->m_prizeFlag, buyType,
            ((DB_BillingRecordSigView*)sig)->m_resultCode,
            ((DB_BillingRecordSigView*)sig)->m_resultMessage);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x61ab,
                                  "db insert error prod_sale_entry_073 m_id: %d",
                                  ((DB_BillingRecordSigView*)sig)->m_mId);
        }
    }
