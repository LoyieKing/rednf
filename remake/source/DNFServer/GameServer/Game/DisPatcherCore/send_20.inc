void DisPatcher_GetItem::send(CUser* user, ParamBase& param)
{
    stGetItemParam& p = static_cast<stGetItemParam&>(param);
    // ORIG 0x081c3ce8：==0x7fffffff → void early-return（call;jmp;nop;merge 岛）；
    // 余下 ==0 走 Ok，else Error。
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x2e);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2e,
                                 (unsigned char)(p.m_errNo & 0xff));
    }
