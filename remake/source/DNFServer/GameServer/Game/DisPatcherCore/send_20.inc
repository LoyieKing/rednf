// ORIG 0x081c3ce8 T _ZN18DisPatcher_GetItem4sendEP5CUserR9ParamBase
// （报告 DisPatcher_GetItem/send.md 逐块映射）：param.m_errNo(+0x4)
// !=0x7fffffff；==0 → SendCmdOkPacket(0x2e)；否则 SendCmdErrorPacket(0x2e, err&0xff)。
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
}
