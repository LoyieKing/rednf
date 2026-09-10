int Dispatcher_UseCraneStart::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b23S3C] ORIG 0x0821d2c4：死预清三槽（-0x1c/-0x18 int 0 + -0x1e short -1，
    // P197③：4.4 -O0 保留未用初始化槽）；err/itemIdx 各一名槽（-0x10/-0x14）；
    // mgr/inven 无槽链式（P48 每点 G_CGameManager→GetCraneMinigameManager 单表达式）；
    // slot 值 movswl 升格 %ebx 跨 call（P200）。EH 选择器/landing 版差残余记 N。
    if (user->get_state() != 3)
        return 0;
    int craneDummyA = 0;                                   // ORIG -0x1c 死预清
    int craneDummyB = 0;                                   // ORIG -0x18 死预清
    short slot = -1;                                       // -0x1e
    if (!packet.get_short(slot))
    {
        return LineFunc(0xd0e8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    try
    {
        int itemIdx = 0;                                   // ORIG -0x14 预清后覆写
        int err = G_CGameManager()->GetCraneMinigameManager()->startUseMaterial(
            user, (int)slot);
        if (err != 0)
            throw static_cast<ENUM_ERROR>(err);            // __cxa_throw 0x0821d3a0
        Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slot);
        itemIdx = item.m_addInfo2;                         // ORIG -0x62=item+0x07
        user->setUseCraneStart(true);                      // 0x0868dd18
        PacketGuard guard;                                 // 0x0858dd4c
        guard.put_header(1, 0x1e6);
        guard.put_byte(1);
        guard.put_short((int)slot);
        guard.put_int(itemIdx);
        guard.finalize(true);
        user->Send(guard);
        return 0;                                          // guard 析构 0x0858de80
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e6, (unsigned char)e);
        return 0;
    }
