void Dispatcher_SelectStriker::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e42ee T：p@-0xc 基址槽；三处各自 `mov -0xc,%eax; mov 0x4(%eax),%eax`
    // 重读字段（寄存器 cmp 不折 cmpl）；OK 段 fall 在前（技巧17）。
    stSelectStriker* p = static_cast<stSelectStriker*>(&param);
    if (p->m_err == (int)0x7fffffff)
        return;
    if (p->m_err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1e8);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e8, (unsigned char)p->m_err);
}