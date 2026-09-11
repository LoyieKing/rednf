// ORIG 0x0842ca06（_ZN22DB_InsertPVPPlayerRate8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；本地 int[6] 清零；逐项 *stream>>；channel_no 取
// G_CEnvironment()->get_channel_no()；inSert log_pvp_player_rate(...)
// values(%d,now(),%d×6)；返 exec(true)==1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842ca06
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
    db->set_query(
        "inSert into log_pvp_player_rate(channel_no,occ_date,single_one_to_one,single_one_to_one_other,team_one_to_one,team_one_to_one_other,relay_one_to_one,relay_one_to_one_other) values(%d,now(),%d,%d,%d,%d,%d,%d)",
        // [Q5W8LT 2026-09-11 落地 P329 实参位内联] ORIG 0x0842ca06 无 channelNo 具名槽
        // （`call G_CEnvironment; call get_channel_no; mov %eax,0x8(%esp)`），且 v[0..5]
        // 在 call **之前**取出（v5→-0x44 / v4→-0x40 / v3→-0x3c / v2→%edi / v1→%esi /
        // v0→%ebx）⇒ channel_no 是实参位内联的**最后求值**表达式；删具名局部后
        // 本件 DIFF→**AE**（80=80 归一）。
        G_CEnvironment()->get_channel_no(), vals[0], vals[1], vals[2], vals[3],
        vals[4], vals[5]);
    if (!db->exec(true))
        return 0;
    return 1;
}