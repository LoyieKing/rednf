// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822a0a4 orig/ours=255/257 ndiff-blocks=35
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN23Dispatcher_UpgradeCargo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    &_ZN10GlobalData22s_pSecuProtectionFi
int Dispatcher_UpgradeCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe7d8,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe7e2,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // 扩仓券物品校验：槽内数量字段须 > 0
    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 券动作合法性（StackableAction TYPE_UPGRADE_CARGO=0x2b，按 itemIdx 校验）
    if (!user->verifyStackableItemActionPlace(StackableAction::TYPE_UPGRADE_CARGO,
                                              (unsigned int)ticket.m_addInfo))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // P48 去缓存每用点重取：ORIG 0x82a2a（isUpgradable 的 receiver 与
    // GetCapacity 源各独立重调 getCurCharacCargoR，共 4 次；不缓存进具名局部）。
    // ORIG 段直证：扩容步长是**具名量**（`movl $0x10,-0xc(%ebp)` + 后续
    // `add -0xc(%ebp),%ebx`），非内联立即数 ⇒ 补回具名常量（原字面形帧少 4 字节、
    // slot/secu 槽整体上移 4）。
    // [DispG1/38] ORIG 0x0822a23e `movl $0x10,-0xc(%ebp)` ⇒ 非 const 具名量（去 const 阻止常量传播）
    unsigned int kAddCapacity = 0x10;
    if (!user->getCurCharacCargoR()->isUpgradable(
            user->getCurCharacCargoR()->GetCapacity() + kAddCapacity))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a32b..0822a482
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 升级仓库容量（+0x10），失败回错误码 0x13
    if (!user->getCurCharacCargoW()->UpgradeCargo(
            user->getCurCharacCargoR()->GetCapacity() + kAddCapacity))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    // 包 2（0x42）：新容量通知
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(1);
    pg.put_short(user->getCurCharacCargoR()->GetCapacity());
    pg.finalize(true);
    user->Send(pg);
    return 0;
}