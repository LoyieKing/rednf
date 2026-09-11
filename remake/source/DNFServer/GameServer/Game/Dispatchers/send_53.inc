// ORIG 0x081d3a72：m_resultCode==0 → put_header(1,0x16d)+put_byte(1)+
// finalize(true)；否则 put_header(1,0x16d)+put_byte(0)+
// put_short(m_resultCode)+finalize(true)。两分支均 CUser::Send。
void Dispatcher_Break_Away_Quest_Check::send(CUser* user, ParamBase& param)
{
    // [b23S3C] ORIG 0x081d3a72：判定在 guard ctor 之前、if/else 各持独立
    // PacketGuard（-0x18/-0x24 双对象+双 EH landing），抄 Skill_Command_All_Default
    // ::send 同形孪生。
    const stBreakAwayParam& res = static_cast<const stBreakAwayParam&>(param);
    if (res.m_resultCode == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x16d);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x16d);
        packet.put_byte(0);
        packet.put_short(res.m_resultCode);
        packet.finalize(true);
        user->Send(packet);
    }
}