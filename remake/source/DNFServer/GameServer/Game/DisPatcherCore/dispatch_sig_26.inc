int DisPatcher_SetPVPMapIndex::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 6)
    {
        return 0;
    }
    bool flag;
    PacketGuard guard;
    // [SigB] P161/P311 组件视图：ORIG 0x081f8634 `lea -0x1f(%ebp),%eax; add $0xd,%eax`
    // （两步取址＝pack(1) 消息视图成员）+ 0x081f8xxx `movzwl -0x12(%ebp),%eax` 重读
    // ⇒ 原码是 pack(1) 视图局部（base -0x1f、m_mapIdx@+0xd、sizeof 0xf），非裸 short。
    // 我方原裸 short 使该局部在 -0x22 且无两步取址（同 IncreStatus 0x081ede5b 形）。
    // ⚠不可用 `__attribute__((packed))`：GCC4.4 拒绝「紧实字段绑定到 short&」
    // （父泳道 00:4x 编译失败实证）；改 `#pragma pack` 形（与聚合器 IncreStatusSigView 同法，
    // probe /tmp/probe_pack.cpp 实证：pragma 形可绑引用，attribute 形报错）。
#pragma pack(push, 1)
    struct SetPVPMapIndexMsgView
    {
        char m_head[0xd];
        short m_mapIdx;
    };
#pragma pack(pop)
    SetPVPMapIndexMsgView sig;
    // [H5DisPatcher 兜底] 4.4 禁止对 packed 成员取址/绑定（`&sig.m_mapIdx` 与
    // `*reinterpret_cast<short*>(&sig.m_mapIdx)` 均报「不能将紧实的字段…绑定到
    // 'short int&'」）。改为「结构体基址 + 0xd 偏移」取址 ⇒ 仍复现 ORIG
    // 0x081f8634 `lea -0x1f(%ebp),%eax; add $0xd,%eax` 的两步取址形态。
    if (!packet.get_short(*reinterpret_cast<short*>(reinterpret_cast<char*>(&sig) + 0xd)))
    {
        return LineFunc(0x8650,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x8653,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    flag = false;
    if (!room->CheckPvPMapMode(user, sig.m_mapIdx, flag))
    {
        guard.put_header(1, 0x3e);
        guard.put_byte(0);
        guard.put_int(0x13);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    room->SetRequestedMapNo(sig.m_mapIdx);
    room->make_state_info(reinterpret_cast<char*>(&guard));
    G_GameWorld()->send_all(guard);
    // [b28d-29] P168a：ORIG 0x081f87d6 `movzbl flag; xor $0x1,%eax; test %al,%al`
    // ⇒ 原码是裸 `!flag`（bool 取反物化），`flag != true` 使 4.4 反产 test/sete。
    if (!flag && room->get_pvp_battle_mode() == 4)
    {
        int res = room->set_pvp_mode(user, 5);
        if (0 < res)
        {
            guard.put_header(1, 0x3e);
            guard.put_byte(0);
            guard.put_byte(0x13);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
    }
    return 0;
}
