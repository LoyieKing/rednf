CUser* PvP_Room::destroy_room(int idx)
{
    // ORIG 0x085d81d0：get_index() 在 CSwitchLog 构造前求值并以 callee-saved
    // 跨构造调用缓存（ORIG +0x07..+0x12），日志首参为 __PRETTY_FUNCTION__
    // 局部静态符号（_ZZ…E19__PRETTY_FUNCTION__，P118/P139）。
    register int roomIdx = get_index();
    CUser* local_14;
    {
        CSwitchLog local_24(__PRETTY_FUNCTION__, 0x721, 0, 0);
        local_24("pvp@log room(%d), %d", roomIdx, idx);
    }
    local_14 = P_SEATS(idx);
    if (local_14 == 0)
    {
        for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_14 = P_SEATS(local_10);
                break;
            }
        }
    }
    reset();
    return local_14;
}