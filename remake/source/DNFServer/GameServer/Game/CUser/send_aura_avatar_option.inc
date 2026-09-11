void CUser::send_aura_avatar_option()
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x17a);
    packet.put_byte(3);
    for (int idx = 0; idx < 3; ++idx) {
        packet.put_byte((char)idx);
        int now = GlobalData::s_systemTime_.getCurSec();
        // [b23S3C] P48/P0：ORIG 双臂各重取 getCurCharacR（无 cur/expire 槽），
        // if/else 分支岛（P56/P198），put_int 两分支各一次
        if (now < getCurCharacR()->m_auraValues[idx])
            packet.put_int(getCurCharacR()->m_auraValues[idx] - now);
        else
            packet.put_int(0);
    }
    packet.finalize(true);
    Send(packet);
}