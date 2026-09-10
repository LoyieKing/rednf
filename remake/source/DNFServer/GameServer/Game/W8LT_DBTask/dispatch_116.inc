bool DB_InsertPartyType::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int v[5] = { 0, 0, 0, 0, 0 };
    // [FL3W8LT 2026-09-10 P329/P0] 原具名 `channelNo` 局部已删：ORIG 0x0842c95b 段
    // 在 `call G_CEnvironment` **之前**就把 v[0..4] 全取出（v[0]→%ebx / v[1]→%esi /
    // v[2]→%edi / v[3]→-0x3c / v[4]→-0x40，右→左 varargs 保活），说明 channel_no 是
    // **实参位内联表达式**（最后一个求值）而非具名局部；具名使 4.4 先算 channelNo
    // 再取 v，并多 push %edi/帧 +0x1c。
    int i = 0;
    while (i <= 4)
    {
        *stream >> v[i];
        ++i;
    }
    db->set_query(
        "inSert into log_party_type(channel_no,occ_date,guild_count,member_count,helped_count,normal_count,member_helped_count) values(%d,now(),%d,%d,%d,%d,%d)",
        G_CEnvironment()->get_channel_no(), v[0], v[1], v[2], v[3], v[4]);
    if (db->exec(true) != 1)
        return 0;
    return 1;
