int PvP_Room::ClearPvPRoom(CUser* user)
{
    destroy_room(0);
    G_CGameManager()->PutPvp(this);
    PacketGuard local_18;
    local_18.clear();
    make_state_info((char*)&local_18);
    G_GameWorld()->send_all(local_18);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}