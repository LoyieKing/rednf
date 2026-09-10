bool DB_InsertPVPPlayerRate::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // [SweepIntervalSet] ORIG 0x0842ca1x 的 24 字节清零是 4.1 对**数组零初始化**
    // 的展开（`lea; mov $0x18,%eax; and; mov $0,%eax; mov %ecx,(%edx,%eax,1);
    // add $4; cmp; jb` 循环形），而非 for 逐元素赋值（4.4 后者产 setle/test/jne）。
    int vals[6] = {0};
    for (int i = 0; i < 6; ++i)
        *stream >> vals[i];
    int channelNo = G_CEnvironment()->get_channel_no();
    db->set_query(
        "inSert into log_pvp_player_rate(channel_no,occ_date,single_one_to_one,single_one_to_one_other,team_one_to_one,team_one_to_one_other,relay_one_to_one,relay_one_to_one_other) values(%d,now(),%d,%d,%d,%d,%d,%d)",
        channelNo, vals[0], vals[1], vals[2], vals[3], vals[4], vals[5]);
    if (!db->exec(true))
        return 0;
    return 1;
