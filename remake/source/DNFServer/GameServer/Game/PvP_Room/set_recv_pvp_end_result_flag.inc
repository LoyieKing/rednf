int PvP_Room::set_recv_pvp_end_result_flag(CUser* user)
{
    // ORIG 0x085dbfec：seat<0 早退 mov $0（fall 侧），显式 if/return 物化，
    // 非 bool 算术化（避免 not/shr 折叠）。
    int seat = get_user_seat(user);
    if (seat < 0)
    {
        return 0;
    }
    m_recvFlagBC.SetRecvFlag(seat);
    return 1;
}