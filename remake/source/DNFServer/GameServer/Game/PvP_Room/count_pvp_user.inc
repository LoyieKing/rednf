int PvP_Room::count_pvp_user()
{
    // ORIG 0x085da840：continue 形产生 true 尾 jmp merge + false nop 岛。
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
        {
            continue;
        }
        local_c = local_c + 1;
    }
    return local_c;
}