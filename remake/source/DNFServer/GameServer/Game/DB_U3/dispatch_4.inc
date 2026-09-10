bool DB_Insert0712WinterEventCoupon::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_INSERT_0712_WINTER_EVENT_COUPON* sig =
        stream->GetOutBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>();

    if (userIdx == 0x112)
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate event_goldcard_info set coupon=coupon+%d where m_id=%s",
            reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
            NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_goldcard_info(m_id,coupon) values(%s,%d)",
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
            db->exec(true);
        }
    }
    else
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate event_goldcard_log set cnt=cnt+%d where m_id=%s and occ_date=now()",
            reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
            NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_goldcard_log(m_id,cnt,occ_date) values(%s,%d,now())",
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
            db->exec(true);
        }
    }
    return true;
