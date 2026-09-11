int DisPatcher_EndPvPResult::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 7)
    {
        return 0;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x8681, __PRETTY_FUNCTION__, 0, 0);
    }
    room->set_recv_pvp_end_result_flag(user);
    // [SigB] P292 正形：ORIG 0x081f892f `test %al,%al; je`（裸 bool 门禁）——
    // `!= 0` 会把 bool 提升为 int，4.4 反产 `test %eax,%eax; setne %al; test %al,%al`。
    // [b28e-DispCore] 负结论：`(unsigned char)` 调用点收窄实测**零效果**
    // （仍 test %eax，130/133 ndiff7 不变）⇒ 属 PvP_Room.h callee int→bool 轴
    // （他泳道），本 TU 判 N。
    if (room->recv_pvp_end_result_all())
    {
        room->end_pvp_result();
        PacketGuard guard;
        room->make_state_info(reinterpret_cast<char*>(&guard));
        G_GameWorld()->send_all(guard);
        room->end_pvp();
        room->battle_reset();
        return 0;
    }
    // [SigB] 同上：ORIG 0x081f89bd/0x081f89d3 皆 `test %al,%al; je`。
    if (room->isRematchable() &&
        room->recv_pvp_end_result_team(user))
    {
        // [SigB] P59/P222 正/负极性包裹 + 块序：ORIG 0x081f89f0 `test %eax,%eax;
        // je 081f8a22 <+0x174>`——取反跳 **log 段**，vtable 直调走 fall-through
        // （0x081f89f4..0x081f8a1c），log 段位于 +0x174。原 `m_matching == 0`
        // 正形使其反产 `jne vtable` + log 前置（块序全反）。
        if (room->m_matching != 0)
        {
            // ORIG vtable+0x80 槽直调（与 PvP_Room.cpp 同款 m_matching 槽直调口径）
            (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x80))(
                (void*)room->m_matching, room, user);
        }
        else
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__, 0x86b6,
                                  "error end pvp result team");
        }
        return 0;
    }
    if (room->get_recv_pvp_end_result_count() == 1)
    {
        room->insert_timer_recv_pvp_end_result();
    }
    return 0;
}
