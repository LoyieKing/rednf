bool DB_UpdateVendingMachineStatistic::dispatch(int userIdx, int slot,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int cnt = 0;
    unsigned int loop = 0;
    *stream >> cnt;
    *stream >> loop;
    if (cnt != 0)
    {
        int channelNo = G_CEnvironment()->get_channel_no();
        db->set_query(
            "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
            channelNo, 0, cnt);
        db->exec(true);
        for (unsigned int i = 0; i < loop; ++i)
        {
            unsigned long itemId = 0;
            unsigned int itemCnt = 0;
            *stream >> itemId;
            *stream >> itemCnt;
            if (itemId == 0)
                return 1;
            channelNo = G_CEnvironment()->get_channel_no();
            db->set_query(
                "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
                channelNo, itemId, itemCnt);
            db->exec(true);
        }
    }
    return 1;
