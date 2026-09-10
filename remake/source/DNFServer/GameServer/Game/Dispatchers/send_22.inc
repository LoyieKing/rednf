void Dispatcher_ChangeLetterStat::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ce304 T：param 指针→-0xc 槽；p->m_4==0x7fffffff/0 → 不发包（je end 共享），
    // 否则 SendCmdErrorPacket(0x89, (unsigned char)p->m_4)（真值 test 形）。
    Err4ParamView* p = reinterpret_cast<Err4ParamView*>(&param);
    if (p->m_4 == (int)0x7fffffff)
        return;
    if (p->m_4)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x89, (unsigned char)p->m_4);
