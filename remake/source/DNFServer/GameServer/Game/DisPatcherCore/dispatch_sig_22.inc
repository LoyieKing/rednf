int DisPatcher_SetPVPTeam::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 6)
    {
        return 0;
    }
    PacketGuard guard;
    // [DPC-SigC 2026-09-11] 负结论（实测劣化，已回滚）：ORIG 0x081f6061
    // `lea -0x1e(%ebp),%eax; add $0xd,%eax`（msg 视图 base -0x1e、成员 @+0xd = -0x11、
    // sizeof 0xe），读取处 0x081f60xx 为单条 `movzbl -0x11(%ebp),%eax`；原独立 `char team`
    // 使帧 0x30（ORIG 0x40）。试形 pack(1) 视图 + 「基址 + 0xd」取址 ⇒ 父泳道复编
    // 实测 137→**145（劣化 +8）** ⇒ 回滚为独立 char（原 137/137 持平）。
    char team;
    if (!packet.get_byte(team))
    {
        return LineFunc(0x82be,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82c3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (team == 4)
    {
        PvPMapInfo info;
        // [G5-SigA] P0/P329：ORIG 0x081f60eb `mov -0x10(%ebp),%eax; mov %eax,(%esp);
        // call GetRequestedMapNo` 之后结果**不经槽**，0x081f60f6 直接
        // `lea -0x34(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp);
        // call GetPvpMapInfo`（两次 call 之间 eax 保活）⇒ 原码是单语句
        // GetPvpMapInfo(room->GetRequestedMapNo(), info)，无 mapIdx 具名局部。
        // 原具名 int mapIdx 多出「store eax→槽 + load 槽→实参」2 条。
        CMapList::GetPvpMapInfo(room->GetRequestedMapNo(), info);
        if (!info.m_playable)
        {
            return LineFunc(0x82ca,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    int result = room->set_pvp_mode(user, (int)(signed char)team);
    // [DPC-SigC 2026-09-11] 负结论（实测劣化，已回滚）：ORIG 0x081f61xx
    // `cmpl $0x0,-0xc(%ebp); jle <make_seat_info 块=+0x1ad>`（成功块 fall-through、
    // 拒绝块在后）。试形 `if (0 < result) {成功}` + 拒绝落尾与上方视图改动合并复编
    // ⇒ 父泳道实测劣化 +8 ⇒ 一并回滚（另修回原缺 `return 0;` 的尾块）。
    if (result < 1)
    {
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
        return 0;
    }
    guard.put_header(1, 0x39);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
