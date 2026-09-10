void Dispatcher_Direct_Entrance_Quick_Party::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081de3b8 T：param 指针→-0xc 槽；p->m_4==0 → SendCmdOkPacket(0x1be)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)p->m_4)（字段二次单级读）。
    Err4ParamView* p = reinterpret_cast<Err4ParamView*>(&param);
    if (p->m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1be);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1be, (unsigned char)p->m_4);
