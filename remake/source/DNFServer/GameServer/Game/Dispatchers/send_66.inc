void Dispatcher_Skill_Command_All_Default::send(CUser* user, ParamBase& param)
{
    // [b23S3C] ORIG 0x081d2f34：param 引用槽(-0xc)回读 m_4 判定；if/else 各自
    // 独立 PacketGuard scope（-0x18/-0x24 两对象+独立 EH landing，抄同 TU 已闭合
    // 孪生 Skill_Command_Customizing::send@26779 形）。
    stSkillCommandParam& res = static_cast<stSkillCommandParam&>(param);
    if (res.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x14d);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x14d);
        packet.put_byte(0);
        packet.put_short(res.m_4);
        packet.finalize(true);
        user->Send(packet);
    }
}