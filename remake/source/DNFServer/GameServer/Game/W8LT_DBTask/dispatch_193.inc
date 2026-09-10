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
        // [FL3W8LT 2026-09-10 P329/P0] 原具名 `channelNo`（两处赋值）已删：ORIG
        // 0x08434718 无 channelNo 槽，`mov -0x14(%ebp),%ebx`（cnt）后直接
        // `call G_CEnvironment; call get_channel_no; mov %eax,0x??(%esp)` ⇒ 实参位
        // 内联（右侧 varargs 求值序保持），删具名同时消掉我方多出的帧 0x48 vs 0x40。
        db->set_query(
            "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
            G_CEnvironment()->get_channel_no(), 0, cnt);
        db->exec(true);
        for (unsigned int i = 0; i < loop; ++i)
        {
            unsigned long itemId = 0;
            unsigned int itemCnt = 0;
            *stream >> itemId;
            *stream >> itemCnt;
            if (itemId == 0)
                return 1;
            db->set_query(
                "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
                G_CEnvironment()->get_channel_no(), itemId, itemCnt);
            db->exec(true);
        }
    }
    return 1;
