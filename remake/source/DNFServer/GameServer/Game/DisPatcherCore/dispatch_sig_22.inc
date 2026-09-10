int DisPatcher_SetPVPTeam::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 6)
    {
        return 0;
    }
    // [K6-DpcSigSmallCheckErr] 改前形 = 独立 `char team`（1 字节，-0x1d；帧 0x30）
    // → 改后形 = `#pragma pack(push,1)` 消息视图（0xe 字节）+ 「基址 + 0xd」取址，
    // 且声明序调整为 视图 → guard → info（ORIG 槽序 视图 -0x1e..-0x11 / guard
    // -0x2c..-0x1d / info -0x34..-0x29，即先声明者占高地址 P331）。依据 ORIG
    // 0x081f6061 `lea -0x1e(%ebp),%eax; add $0xd,%eax`（两步取址 ⇒ pack(1) 视图 base
    // -0x1e、成员 @+0xd = -0x11、sizeof 0xe；帧 0x40）与读取处单条
    // `movzbl -0x11(%ebp),%eax`。旧试形（未调声明序）实测 137→145 劣化 ⇒ 本次连带
    // 声明序一起对齐。⚠不可用 `__attribute__((packed))`（4.4 拒绝绑定紧实字段到引用）。
#pragma pack(push, 1)
    struct SetPVPTeamMsgView
    {
        char m_head[0xd];
        char m_team;
    };
#pragma pack(pop)
    SetPVPTeamMsgView sig;
    PacketGuard guard;
    if (!packet.get_byte(
            *reinterpret_cast<char*>(reinterpret_cast<char*>(&sig) + 0xd)))
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
    if (sig.m_team == 4)
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
    int result = room->set_pvp_mode(user, (int)(signed char)sig.m_team);
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
