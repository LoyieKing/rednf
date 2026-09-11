// CBattle_Field::BloodRoundMaxTimePacket —— ORIG 0x08305dc4（0xd8 字节）
//   报告：docs/class_func_reports/CBattle_Field/BloodRoundMaxTimePacket.md
//   header 0xd6 = 大轮+1、最大时间、秒数；经 m_party 广播。
void CBattle_Field::BloodRoundMaxTimePacket(short round, int a, int b)
{
    PacketGuard packet;
    packet.put_header(0, 0xd6);
    packet.put_byte(0);
    packet.put_byte((int)round + 1);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    m_party->send_to_party(packet);
}