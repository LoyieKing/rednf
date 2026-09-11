bool PvP_Room::set_recv_pvp_rank_flag(CUser* user)
{
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    if (m_recvFlagB4.IsRecv(iVar2))
    {
        return 0;
    }
    m_recvFlagB4.SetRecvFlag(iVar2);
    return 1;
}