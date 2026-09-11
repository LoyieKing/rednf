// ORIG 0x081c9a14 T _ZN22DisPatcher_SetPVPReady7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_SetPVPReady::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {   // LineFunc(0x1ce3, pretty, 0, acc_id)
        return LineFunc(0x1ce3,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {   // 无房间：LineFunc(0x1ce6, pretty, 0, acc_id)
        return LineFunc(0x1ce6,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    if (room->get_pvp_battle_mode() == 6)   // 围观/观战模式拒绝
    {
        return -1;
    }
    stOnSetPvPReadyParam& out = static_cast<stOnSetPvPReadyParam&>(param);
    MSG_SET_READY_STATE& req = *reinterpret_cast<MSG_SET_READY_STATE*>(&msg);
    out.m_errNo = room->set_ready_state(user, req.m_0d != 0);
    if (out.m_errNo < 0)
    {   // 全员就绪 → 开赛并广播状态包
        room->start_pvp();
        PacketGuard guard;                       // ORIG local_28（栈上 48B）
        room->make_state_info(reinterpret_cast<char*>(&guard));
        G_GameWorld()->send_all(guard);
        return -1;
    }
    return err;   // 该分支 err==0，与 ORIG `mov -0x18(%ebp),%ebx` 一致
}
