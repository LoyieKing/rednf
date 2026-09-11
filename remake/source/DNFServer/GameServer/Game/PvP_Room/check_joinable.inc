int PvP_Room::check_joinable(CUser* user)
{
    // [b28d-7] P16 早返结构：ORIG 0x085dbd3c `je BODY; mov$4; jmp END; BODY:`
    // ⇒ 失败块（return 4）物理居前，if/else 形反产 jne+双 return 4 尾。
    if (m_state != 1)
    {
        return 4;
    }
    int local_10 = 0;
    if ((local_10 = ((IMatch*)m_matching)->CheckJoinable(this, user)) != 0)
    {
        return local_10;
    }
    for (int local_f4 = 0; local_f4 < 8; local_f4 = local_f4 + 1)
    {
        if ((P_SEATS(local_f4) == 0) && (m_teams[local_f4] == 0xff))
        {
            return 0;
        }
    }
    return 4;
}