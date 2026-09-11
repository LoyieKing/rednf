void CUser::send_pvp_record()
{
    PacketGuard packet;
    packet.put_header(0, 0x30);
    // ORIG：getPVPResultRefR 每处 put 独立重调 7 次——访问器结果禁缓存
    // （战术 P183/P48）。
    packet.put_int(getPVPResultRefR()->m_totalWinCount);
    packet.put_int(getPVPResultRefR()->m_totalLoseCount);
    packet.put_int(getPVPResultRefR()->m_expPoint);
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpCurrRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpNextRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_byte(getPVPResultRefR()->m_pvpGrade);
    // ORIG 0x: `mov 0x5c(%eax),%eax` 32 位直读 m_todayWinCount(int@+0x5c) 后
    // 交 put_byte(int)——用字节指针 `*(unsigned char*)&` 会被 4.4 折成
    // add+movzbl 载低字节（多 3 条），改传 int 直读对齐 ORIG。
    packet.put_byte(getPVPResultRefR()->m_todayWinCount);
    packet.finalize(true);
    Send(packet);
}