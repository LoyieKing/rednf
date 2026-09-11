// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x082231ae orig/ours=97/97 ndiff-blocks=10
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: movb   $0x0,-0x11(%ebp) | movw   $0x0,-0x14(%ebp) | lea    -0x11(%ebp),%eax
int Dispatcher_RequestItemUnlockCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd7b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd7c, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x19（ItemUnlockCancel 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x19);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)secu);
        return 0;
    }
    int able = 0;                                    // ORIG：调用前 -0xc 预清零
    able = user->IsAbleItemUnlockCancel(slot, itemIdx);
    // ORIG 0x082232d8：`cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`
    //   —— bool 归一岛、**无具名 bool 槽**（我方原 `bool nz` 落 -0x9(%ebp)，
    //   多出 `mov %al,-0x9; cmpb $0x0,-0x9` 两条 + 1 字节槽）。
    while (able != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)able);
        return 0;
    }
    user->DoItemUnlockCancel(slot, itemIdx);
    return 0;
}