// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0822a4bc orig/ours=275/264 ndiff-blocks=39
//   帧差 sub $8c vs $4 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x8c,%esp | movl   $&_ZZN27Dispatcher_UpgradeInventory12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
// [SweepDispatchers3 2026-09-09] 修真缺口：删 invenR/capOld/capNew 缓存，每个使用点
//   重新 getCurCharacInvenR()/get_inventory_capacity()（P160）⇒ ORIG 的 9 次/6 次调用
//   归位。gap 11→0（275=275），DIFF→AE（报告 dispatch_sig.md Ghidra C 已读）。
int Dispatcher_UpgradeInventory::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe828,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe832,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 扩背包券仅两种道具 idx：0x2897ca / 0x203b
    if (!(ticket.m_addInfo == 0x2897ca || ticket.m_addInfo == 0x203b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 背包容量上限 16 格
    if (user->getCurCharacInvenR()->get_inventory_capacity() > 0xf)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a71a..0822a8db
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 扩容日志（旧容量 → +8），再落实际容量与 DB 请求。ORIG（0x0822a7xx 段）每个
    // 使用点都重新 getCurCharacInvenR()/get_inventory_capacity()（P160 禁缓存 getter）。
    user->m_historyLog.ExpandInventory(
        user->getCurCharacInvenR()->get_inventory_capacity(),
        user->getCurCharacInvenR()->get_inventory_capacity() + 8, 1);
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(
        user->getCurCharacInvenR()->get_inventory_capacity() + 8);
    DB_UpdateInventoryCapacity::makeRequest(
        user->GetServerGroup(), (int)user->GetUID(),
        (unsigned int)user->getCurCharacNo(),
        user->getCurCharacInvenR()->get_inventory_capacity());

    // 包 2（0x42）：subtype 0xb + 新容量
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(0xb);
    pg.put_short(user->getCurCharacInvenR()->get_inventory_capacity());
    pg.finalize(true);
    user->Send(pg);
    return 0;
}