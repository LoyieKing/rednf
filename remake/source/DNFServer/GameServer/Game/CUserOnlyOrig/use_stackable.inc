// [b28d-22 N82] ORIG 0x0865e0ae：p167=M0/E2:_Z25GetInvenTypeFromItemSpace14ENUM_ITEMSPACE，gap=17；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0865e0ae：N82 封顶——帧/槽差（p167 M0/E2，gap 17）。
bool CUser::use_stackable(short slot, int space, int p3, int p4)
{
    int err = 0;
    if (CheckInTrade()) {
        err = 0x13;
    }
    Inven_Item item = const_cast<CInventory*>(getCurCharacInvenR())
                          ->GetInvenSlot(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot);
    CItem* itemDef = G_CDataManager()->find_item(item.m_addInfo);
    if (itemDef == 0) {
        err = 0x17;
    }
    else if (!itemDef->is_stackable()) {
        err = 0x17;
    }
    // [CoBatchD 2026-09-11 消 && 物化] ORIG 0x0865e160 `cmpl $0x0,-0x10(%ebp); jne
    // 0865e27f` + 0x0865e17f `test %al,%al; je 0865e27f` —— 两个失败口**直跳同一目标**，
    // 无 `mov $1/%eax` 汇合物化（4.4 对 `&&` 产 `mov $0x1,%eax; jmp / mov $0x0,%eax`）。
    if (err == 0)
    if (expert_job::CAlchemist::CheckUsableStackableItem(this, itemDef) != 1) {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        packet.put_byte(0);
        packet.put_byte(7);
        packet.put_byte(space);
        packet.put_int(p3);
        packet.put_int(p4);
        packet.finalize(true);
        Send(packet);
        return true;
    }
    // 同上：ORIG 同段第二处 `&&` 亦为直跳形（0x0865e1xx 族）。
    if (err == 0)
    if (CheckCoolTimeItem(static_cast<unsigned long>(item.m_addInfo))) {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.put_byte(space);
        packet.put_int(p3);
        packet.put_int(p4);
        packet.finalize(true);
        Send(packet);
        return true;
    }
    if (err == 0) {
        CInventory* inven = getCurCharacInvenW();
        err = inven->use_item(slot, 1, space);
    }
    if (err == 0) {
        Global::CBossTowerFunc::UseItem(this, item);
    }
    if (err == 0) {
        _processContinuousItem(static_cast<unsigned int>(item.m_addInfo),
                               static_cast<CStackableItem*>(itemDef));
    }
    if (err == 0) {
        expert_job::CAlchemist::OnUseStackableItem(
            this, static_cast<unsigned long>(item.m_addInfo), itemDef);
    }
    if (err == 0) {
        // [M8COA 2026-09-11 一试·switch 复现 ORIG 决策树] ORIG 判值为**寄存器**
        // （0x0865e2xx `cmp $0x1e70,%eax; je …; cmp $0x1ddd,%eax; ja …`，多次同
        // 寄存器比较 = 4.1 二分决策树）；旧稿手写 if/else 树被 4.4 lower 成
        // `cmpl $K,-0xc(%ebp); jne`（槽判值）顺序链 ⇒ 按 increase_status 先例
        // （P251/P261/P286/P324：4.4.7 对 switch(局部) 复现 4.1 二分树）改写真 switch。
        switch (static_cast<unsigned int>(item.m_addInfo)) {
        case 0x1e70:
            SendLV70Buff(this, 0x1e77);
            break;
        case 0x1d20: {
            int remain = getCurCharacFatigue() - 0x28;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
            break;
        }
        case 0x1d5e: {
            int remain = getCurCharacFatigue() - 10;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
            break;
        }
        case 0x1ddc: case 0x1ddd: {
            int remain = getCurCharacFatigue() - 6;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
            break;
        }
        case 0x1e6d: SendLV70Buff(this, 0x1e74); break;
        case 0x1e6e: SendLV70Buff(this, 0x1e75); break;
        case 0x1e6f: SendLV70Buff(this, 0x1e76); break;
        case 0x1e71: SendLV70Buff(this, 0x1e78); break;
        case 0x1e72: SendLV70Buff(this, 0x1e79); break;
        case 0x1e73: SendLV70Buff(this, 0x1e7a); break;
        case 0x1f8f: case 0x27ac5a: case 0x27addd: case 0x292090de: {
            int remain = getCurCharacFatigue() - 0x1e;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
            break;
        }
        case 0x201d: {
            int remain = getCurCharacFatigue() - 0x14;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
            break;
        }
        default:
            break;
        }
    }
sendResult:
    {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        if (err != 0) {
            packet.put_byte(0);
            packet.put_byte(err);
            packet.put_byte(space);
            packet.put_int(p3);
            packet.put_int(p4);
            packet.finalize(true);
            Send(packet);
            Inven_Item slotItem = const_cast<CInventory*>(getCurCharacInvenR())
                                      ->GetInvenSlot(1, slot);
            // [FL5 2026-09-10 P37/P227] 全表达式临时链
            cMyTrace(__PRETTY_FUNCTION__, 0x38b5, 5)(
                "CUser::use_stackable\tError:%d\tSlot_no:%d\tItem_no:%d",
                err, static_cast<int>(slot), slotItem.m_addInfo);
        }
        else {
            packet.put_byte(1);
            packet.put_short(static_cast<int>(slot));
            packet.put_byte(space);
            packet.put_int(p3);
            packet.put_int(p4);
            packet.finalize(true);
            Send(packet);
            ARAD::ARAD_ITEM::arad_secret_shop_event_item(
                this, static_cast<unsigned long>(item.m_addInfo),
                static_cast<CStackableItem*>(itemDef));
        }
    }
    return err == 0;
}