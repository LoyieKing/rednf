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
    if (err == 0 && expert_job::CAlchemist::CheckUsableStackableItem(this, itemDef) != 1) {
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
    if (err == 0 && CheckCoolTimeItem(static_cast<unsigned long>(item.m_addInfo))) {
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
        unsigned long idx = static_cast<unsigned long>(item.m_addInfo);
        if (idx == 0x1e70) {
            SendLV70Buff(this, 0x1e77);
        }
        else if (idx < 0x1e71) {
            if (idx < 0x1dde) {
                if (idx < 0x1ddc) {
                    if (idx == 0x1d20) {
                        int remain = getCurCharacFatigue() - 0x28;
                        int zero = 0;
                        setCurCharacFatigue(std::max(zero, remain));
                        SendFatigue();
                    }
                    else if (idx == 0x1d5e) {
                        int remain = getCurCharacFatigue() - 10;
                        int zero = 0;
                        setCurCharacFatigue(std::max(zero, remain));
                        SendFatigue();
                    }
                }
                else {
                    int remain = getCurCharacFatigue() - 6;
                    int zero = 0;
                    setCurCharacFatigue(std::max(zero, remain));
                    SendFatigue();
                }
            }
            else if (idx == 0x1e6e) {
                SendLV70Buff(this, 0x1e75);
            }
            else if (idx < 0x1e6f) {
                if (idx == 0x1e6d) {
                    SendLV70Buff(this, 0x1e74);
                }
            }
            else {
                SendLV70Buff(this, 0x1e76);
            }
        }
        else {
            if (idx != 0x1f8f) {
                if (idx < 0x1f90) {
                    if (idx == 0x1e72) {
                        SendLV70Buff(this, 0x1e79);
                    }
                    else if (idx < 0x1e72) {
                        SendLV70Buff(this, 0x1e78);
                    }
                    else if (idx == 0x1e73) {
                        SendLV70Buff(this, 0x1e7a);
                    }
                    goto sendResult;
                }
                if (idx != 0x27ac5a) {
                    if (idx < 0x27ac5b) {
                        if (idx == 0x201d) {
                            int remain = getCurCharacFatigue() - 0x14;
                            int zero = 0;
                            setCurCharacFatigue(std::max(zero, remain));
                            SendFatigue();
                        }
                        goto sendResult;
                    }
                    if ((idx != 0x27addd) && (idx != 0x292090de)) {
                        goto sendResult;
                    }
                }
            }
            int remain = getCurCharacFatigue() - 0x1e;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
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
