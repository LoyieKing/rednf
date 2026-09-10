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
    if (room->recv_pvp_end_result_all() != 0)
    {
        room->end_pvp_result();
        PacketGuard guard;
        room->make_state_info(reinterpret_cast<char*>(&guard));
        G_GameWorld()->send_all(guard);
        room->end_pvp();
        room->battle_reset();
        return 0;
    }
    if (room->isRematchable() != 0 &&
        room->recv_pvp_end_result_team(user) != 0)
    {
        if (room->m_matching == 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__, 0x86b6,
                                  "error end pvp result team");
        }
        else
        {
            // ORIG vtable+0x80 槽直调（与 PvP_Room.cpp 同款 m_matching 槽直调口径）
            (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x80))(
                (void*)room->m_matching, room, user);
        }
        return 0;
    }
    if (room->get_recv_pvp_end_result_count() == 1)
    {
        room->insert_timer_recv_pvp_end_result();
    }
    return 0;
