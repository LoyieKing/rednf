bool DB_InsertPartyType::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int v[5] = { 0, 0, 0, 0, 0 };
    // [L4-r2 判据⑩] ORIG 槽 v@-0x34 / channelNo@-0x20 / i@-0x1c（4.1 先声明更深）
    // ⇒ channelNo 的声明必须前置（无初始化器 + 赋值留在原位）。
    int channelNo;
    int i = 0;
    while (i <= 4)
    {
        *stream >> v[i];
        ++i;
    }
    channelNo = G_CEnvironment()->get_channel_no();
    db->set_query(
        "inSert into log_party_type(channel_no,occ_date,guild_count,member_count,helped_count,normal_count,member_helped_count) values(%d,now(),%d,%d,%d,%d,%d)",
        channelNo, v[0], v[1], v[2], v[3], v[4]);
    if (db->exec(true) != 1)
        return 0;
    return 1;
