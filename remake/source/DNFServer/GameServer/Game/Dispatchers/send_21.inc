void Dispatcher_MailBox_Extract::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ccf94 T：param 指针→-0xc 槽；p->m_4==0x7fffffff/0 → 不发包（je end 共享），
    // 否则 SendCmdErrorPacket(0x62, (unsigned char)p->m_4)（真值 test 形）。
    stMailboxExtractParam* p = static_cast<stMailboxExtractParam*>(&param);
    if (p->m_4 == (int)0x7fffffff)
        return;
    if (p->m_4)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, (unsigned char)p->m_4);
}