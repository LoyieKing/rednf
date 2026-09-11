// ORIG 0x081f8602 T _ZN25DisPatcher_SetPVPMapIndex12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82 + setcc-fold：帧 0x40 vs 0x30；`!flag` 已按 P168a 修正（xor 形），残余为槽平移与 4.4 sete 折痕  (ORIG 0x081f8602 insn 175->179)
// 报告：docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 59（ORIG 175 / ours 178），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SetPVPMapIndex::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 6)
    {
        return 0;
    }
    // [DPC-SigC 2026-09-11] P331 声明序反转（ndiff 139→93 落地）：ORIG 槽序（高→低地址）
    // 为 sig(-0x1f..-0x11，两步取址 `lea -0x1f; add $0xd`)、guard(-0x2c..-0x21)、
    // flag(-0x2d)，即声明序 sig,guard,flag；我方原声明序 flag,guard,sig 使 GCC 反向分配出
    // sig(-0x2f..-0x22)、guard(-0x20..-0x15)、flag(-0x11)（LCS 约 20 块全为帧偏移差）。
    // 父泳道复编实测 ndiff 139→93（指令数持平、verdict 不跨档）⇒ 落地。
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
    PacketGuard guard;                       // ORIG -0x2c（声明序：sig,guard,flag）
    bool flag;                               // ORIG -0x2d
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
    // [DPC-SigSmall 2026-09-11] 短路轴：ORIG 0x081f8791-0x081f87b3 为
    // `xor $1; test; je <tail>` 后接 `call; cmp $4,%eax; sete %al; test; je <同一 tail>`
    // —— 两跳同指 tail ⇒ 原码是**嵌套 if**（内层条件作值物化 sete），不是 `&&`。
    // probe（/tmp/dpc_probe2.cpp，c6446r 4.4.6 -m32 -O0）：`A && B` 产分支 0/1 岛
    // （jne/mov $1/jmp/mov $0，即旧形 OURS-only 三条），嵌套 if 产 sete/test/je
    // 与 ORIG 逐条同形 ⇒ 落地。
    if (!flag)
    {
        if (room->get_pvp_battle_mode() == 4)
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
    }
    return 0;
}
