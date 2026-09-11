// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x08222f82 orig/ours=144/144 ndiff-blocks=26
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: movb   $0x0,-0x11(%ebp) | movw   $0x0,-0x14(%ebp) | lea    -0x11(%ebp),%eax
int Dispatcher_RequestItemUnlock::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd4c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd4d, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x18（ItemUnlock 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x18);
    if (secu != 0)   // ORIG：无布尔物化，直 cmpl $0,-0x10(%ebp); je
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x110, (unsigned char)secu);
        return 0;
    }
    // 槽位精确版：able@-0xc、err@-0x18、slot@-0x11、itemIdx@-0x14、secu@-0x10。
    // 声明序 able→err：4.1 初始化 store 序 = 声明序（ORIG -0xc 先清）。
    int able = 0;                                    // -0xc
    int err = 0;                                     // -0x18（IsAbleItemUnlock 出参）
    able = user->IsAbleItemUnlock(slot, itemIdx, err);
    // ORIG 0x0822...：`mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp); ...; test %al,%al`
    //   —— 无 `mov %al,-0x9(%ebp)` bool 具名量（我方 anz 多占 1 槽 ⇒ 全槽下移 4）。
    // [G6-90] P295：ORIG 0x08222f82 区 `setne %al; test %al,%al; je`（setcc 物化）
    //   ⇒ 用 while 形逼 4.4 产物化岛（K1 P295 已验证手法）。
    while (able != 0)
    {
        PacketGuard packet_;                         // -0x24
        packet_.put_header(1, 0x110);
        packet_.put_byte(0);
        packet_.put_byte(able);
        packet_.put_int(err);
        packet_.finalize(true);
        user->Send(packet_);
        return 0;
    }
    user->DoItemUnlock(slot, itemIdx);
    return 0;
}