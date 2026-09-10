void Dispatcher_UseRightOfChangeGrowType::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825df24 T：param 指针→-0xc 槽；Error 段 fall-through 在前（技巧17），
    // je → 尾 OK 岛；字段二次单级读。
    Err4ParamView* p = reinterpret_cast<Err4ParamView*>(&param);
    if (p->m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f7, (unsigned char)p->m_4);
    else
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f7);
