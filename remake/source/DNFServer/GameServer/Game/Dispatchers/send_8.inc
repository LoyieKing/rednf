void Dispatcher_BossDieCheck::send(CUser* user, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    if (out.m_c == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb7, 2);
    }
    PacketGuard packet;
    packet.put_header(0, 0x73);
    // ORIG 0x081dxxxx：`test %eax,%eax; je <跳走>` —— **失败块（put_byte 0/put_byte m_4）
    // 才是 fall-through**，成功块在跳远侧（技巧17/23；与 SkillInit/RefundSkill::send 的
    // 方向相反，故必须逐条看 ORIG 那条 je/jne，不能整族批量套）。
    if (out.m_4 != 0)
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    else
    {
        packet.put_byte(1);
        packet.put_byte((unsigned char)out.m_8);
        packet.put_short((unsigned short)out.m_a);
    }
    packet.finalize(true);
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party != 0)                          // ORIG cmpl$0;je（party==0 跳 user->Send，send_to_party fall-through）
    {
        party->send_to_party(packet);
    }
    else
    {
        user->Send(packet);
    }
