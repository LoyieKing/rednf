void Dispatcher_SkillInit::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5aec：`test %eax,%eax; jne <guard 段>` —— 正常路径是 **fall-through**，
    // SendCmdErrorPacket 上报体被摆在**函数物理尾**（技巧17/23：ORIG fall-through 块在前）。
    // 我方原写 `if (m_4 != 0) { 上报; return; } <guard>` 使上报体占了 fall 位 ⇒ 整块顺序反转、
    // ndiff 19。改成正向嵌套即复位。
    stSkillInit& p = static_cast<stSkillInit&>(param);
    if (p.m_4 == 0)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0x1ec);
        pkt.put_byte(1);
        pkt.put_byte((int)(char)p.m_8);
        pkt.finalize(true);
        user->Send(pkt);
        user->send_skill_info();
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ec,
                                 (unsigned char)(p.m_4 & 0xff));
    }
