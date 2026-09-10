int CParty::do_after_dungeon_start_checkndelete_item(const RequiredItem& item,
                                                     CUser* user)
{
    // [b28d-solo-PartyClosureC2 判词·P337] ORIG 0x085a0756。
    // 旧判词 N111「死块不可造」被 P337 推翻：恒 0 **volatile** 守卫的
    // 读取不可省略，块体与 ORIG 7 个调用点强制发射（`if(0)`/goto 才会 DCE）。
    // 死块运行期不可达（与 ORIG 入口 jmp 等价），活动路径语义零变化。
    // 现读数：133 vs 134（gap −1）、p167 19 vs 19（余差仅 PacketGuard/cMyTrace
    // C1↔C2/D1↔D2 别名对）、帧 0x90 vs ORIG 0x80（N82 槽阶梯残余）。
    int slot = 0;                                         // ORIG -0xc（+0x008 movl）
    const CInventory* inv = user->getCurCharacInvenR();   // ORIG +0x1a（返回值仅死块用）

    volatile bool neverEnter = false;                     // P337 恒 0 守卫
    if (neverEnter)
    {
        // ORIG +0x2b：+0x1a 返回值直存 slot 槽再与 -1 比较（按指令转写）
        slot = (int)reinterpret_cast<size_t>(inv);
        const bool deadMissing = (slot == -1);                    // ORIG +0x2e
        if (deadMissing)
            return 0x11;
        const Inven_Item deadItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, slot);    // ORIG +0x43..+0x6c
        if (deadItem.m_addInfo2 < item.m_itemCount)               // ORIG +0x6f..+0x7a
            return 0x11;
        if (!user->getCurCharacInvenW()->delete_item(             // ORIG +0x86..+0xc2
                (INVEN_TYPE)1, slot, item.m_itemCount, (eItemDelReason)0xe, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xdfa, 5);        // ORIG +0xdc..+0xfa
            trace("CParty::isEventDungeon, delete_item failed , User ch=%d , %d %d",
                  user->getCurCharacNo(), slot, item.m_itemCount); // ORIG +0xff..+0x11c
            return 0x11;
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet);
    packet.finalize(true);
    user->Send(packet);
    return 0;
