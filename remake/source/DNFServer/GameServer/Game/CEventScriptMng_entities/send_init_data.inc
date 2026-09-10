bool CAttendance::send_init_data(CUser* user, PacketGuard& packet,
                                 short key, unsigned char count, bool flag)
{
    // 两计数以单表达式按 int 相加（前者经寄存器缓存）后整体传入；return 1。
    packet.put_short(key);
    packet.put_short(
        user->get_server_fatigue_count(count) +
        user->get_fatigue_count_db(count));
    return true;
