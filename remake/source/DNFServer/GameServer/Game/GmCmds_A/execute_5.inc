// ORIG execute 0x084ac58c：重算角色属性后回发 basic_info 包。
void CAdjustCharacStat::execute()
{
    CUser* user = GetUser();
    user->adjust_charac_stat();   // 真实定义 UserPartyLink4.cpp（T 0x08664766）

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 1);
    packet.finalize(true);
    user->Send(packet);
}