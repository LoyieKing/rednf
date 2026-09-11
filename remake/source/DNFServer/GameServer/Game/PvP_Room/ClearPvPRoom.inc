int PvP_Room::ClearPvPRoom(CUser* user)
{
    destroy_room(0);
    G_CGameManager()->PutPvp(this);
    PacketGuard local_18;
    local_18.clear();
    make_state_info((char*)&local_18);
    G_GameWorld()->send_all(local_18);
}