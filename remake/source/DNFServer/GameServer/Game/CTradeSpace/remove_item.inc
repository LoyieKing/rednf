int CTradeSpace::remove_item(CUser* user, int trade_slot_no, int type,
                             int target_slot, int count)
{
    // b28d-35: DIFF→N82（1767/1767 等量；P183 去缓存后 gap=4，§6.2）。
    //   ORIG @0x0852bfcc。
    // ORIG +0x17 `test %al,%al; je`：直判形（4.4 对裸 if(char) 生 setne 物化，
    // 显式 !=0 抑制归一尝试）
    if (IsLocked())
        return -1;
    if (count <= 0)
        return -0x62;

    PacketGuard packet;
    // P331：同尺寸标量区声明序 = 槽深序（4.4 先声明更深）。
    // ORIG 槽：otherUser -0x44（最深）/ other -0x40 / trader -0x3c（最浅），
    // 且两分支 store 序为 otherUser→other→trader（+0x58/+0x5b/+0x62）。
    CUser* otherUser = 0;   // ORIG +0x40 `movl $0x0,-0x44`：先置零
    int other;
    int trader;
    if (m_pTraders[0] == user) {
        otherUser = m_pTraders[1];
        other = 1;
        trader = 0;
    } else {
        otherUser = m_pTraders[0];
        other = 0;
        trader = 1;
    }
    m_nState[trader] = 0;
    m_nState[other] = 0;

    if (otherUser == 0 || trade_slot_no > 0x1a)
        return -1;
    if (m_pTraders[trader]->getCurCharacR() == 0 ||
        user->getCurCharacR() == 0) {
        // P37 单语句链：实参在构造调用前求值（ORIG +0xc8/+0xd8 两 call 先于
        // traceC@0x132），arg1 先 arg2 后
        cMyTrace("int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                 0x45a, 5)
            ("CTradeSpace::remove_item()\t0 == "
             "m_pTraders[owner_index]->getCurCharacR() %x || 0 == "
             "owner->getCurCharacR() %x",
             m_pTraders[trader]->getCurCharacR(), user->getCurCharacR());
        return -1;
    }

    if (m_slotsData[trader][trade_slot_no].m_field08 != type)
        return -1;
    switch (type) {
    case 0:
        if (trade_slot_no == 0) {
            // ---- 金币 ----
            // ORIG +0x1da `js`→负值跳尾块、正值直落（反向条件控布局）
            if (m_nMoney[trader] - count >= 0) {
                user->getCurCharacInvenW()->gain_money(
                    count, (eMoneyAddReason)1, 1, 0);
                m_nMoney[trader] -= count;
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                if (m_nMoney[trader] > 0)
                    ((InterfacePacketBuf*)&packet)->put_int(0);
                else
                    ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(m_nMoney[trader]);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
                m_nState[trader] = 0;
                m_nState[other] = 0;
                return 0;
            }
            return -1;
        }
        {
            // ---- 物品（trade_slot_no != 0）----
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
            int ret = -1;
            int slotItemType =
                m_pTraders[trader]->getCurCharacInvenR()->GetSlotItemType(
                    target_slot);
            if (slotItemType != 0 &&
                (unsigned int)slotItemType != copy.m_item.m_field1)
                return -1;
            if (copy.m_item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP) {
                if (copy.m_field08 != 0)
                    return -1;
                if (m_pTraders[trader]->getCurCharacInvenR()
                        ->check_empty_count(
                            (Inven_Item::ITEM_TYPE)copy.m_item.m_field1, 1)) {
                    ret = m_pTraders[trader]->getCurCharacInvenW()
                              ->insert_item_special_slot(
                                  copy.m_item, count, target_slot,
                                  (eMoneyAddReason)9, (eItemAddReason)0x10);
                    if (ret < 0) {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4a5,
                            "FATAL_ERROR USER[%s] ITEMID[%d] -> SLOT[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo,
                            target_slot);
                        return ret;
                    }
                } else {
                    LogManager::logFormat(
                        1, "item_trade.cpp",
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x4ac, "[TRADE]FATAL_ERROR USER[%s] ITEMID[%d]",
                        user->getCurCharacName(), copy.m_item.m_addInfo);
                    return -1;
                }
            } else {
                // ---- 普通物品 ----
                if (copy.m_item.get_add_info() < count)
                    return -1;
                copy.m_item.set_add_info(count);
                if (count > 0x3e7) {
                    // P227 全表达式临时链：ORIG +0x4b8 前 `mov -S,%ebx`
                    // 先求 vararg 实参并跨 ctor 保活
                    cMyTrace(
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x4b8, 5)
                        ("CTradeSpace::remove_item, moveitem.item.set_add_info "
                         ", ITEM #%d, %d",
                         copy.m_item.m_addInfo, count);
                }
                int itemIdx = m_pTraders[trader]->getCurCharacInvenR()
                                  ->check_item_exist(copy.m_item.m_addInfo);
                if (itemIdx != -1) {
                    ret = m_pTraders[trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  copy.m_item, (eItemAddReason)0x10, 1, 1);
                    if (ret < 0) {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4cc, "[TRADE]FATAL_ERROR USER[%s] ITEMID[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo);
                        return ret;
                    }
                } else {
                    if (m_pTraders[trader]->getCurCharacInvenR()
                            ->check_empty_count(
                                (Inven_Item::ITEM_TYPE)copy.m_item.m_field1,
                                1)) {
                        ret = m_pTraders[trader]->getCurCharacInvenW()
                                  ->insert_item_special_slot(
                                      copy.m_item, count, target_slot,
                                      (eMoneyAddReason)9,
                                      (eItemAddReason)0x10);
                        if (ret < 0)
                            return ret;
                    } else {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4db, "FATAL_ERROR USER[%s] ITEMID[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo);
                        return -1;
                    }
                }
            }
            m_nState[trader] = 0;
            m_nState[other] = 0;
            // ORIG +0x99c `cmpl $-0x1; je 尾块`：!=−1 直落发包，−1 跳尾 return
            if (ret != -1) {
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
            ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
            // ORIG +0x9ff：EQUIP→全清；非EQUIP仅槽内数量>count 时扣减，否则全清。
            if (m_slotsData[trader][trade_slot_no].m_item.m_field1
                    == Inven_Item::ITEM_TYPE_EQUIP) {
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                m_slotsData[trader][trade_slot_no].m_field00 = 0;
                m_slotsData[trader][trade_slot_no].m_field04 = 0;
                m_slotsData[trader][trade_slot_no].m_field08 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
                m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
                m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            } else if (m_slotsData[trader][trade_slot_no].m_item
                           .get_add_info() > count) {
                // 部分移除（ORIG +0xbc3/+0xc0f 两次 get_add_info 调用；
                // newAddInfo 直读槽 m_addInfo2 @+0xc44）
                ((InterfacePacketBuf*)&packet)->put_int(copy.m_item.m_addInfo);
                ((InterfacePacketBuf*)&packet)->put_int(
                    m_slotsData[trader][trade_slot_no].m_item.get_add_info()
                    - count);
                // P198①/P171 两语句：ORIG +0xc44 `mov mem,%eax; mov %eax,-S`
                // 先存槽，再 `mov count,%eax; sub %eax,-S`（store-then-sub mem）
                int newAddInfo =
                    m_slotsData[trader][trade_slot_no].m_item.m_addInfo2;
                newAddInfo -= count;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(
                    newAddInfo);
                if (newAddInfo > 0x3e7) {
                    // P37 链式：槽 m_addInfo 实参先于构造求值（ORIG
                    // +0xcb0 mov 0xb(%eax),%ebx 先于 traceC@0xcd1）
                    cMyTrace(
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x503, 5)
                        ("CTradeSpace::remove_item, "
                         "m_ItemSlot[owner_index][trade_slot_no].item."
                         "set_add_info 2 , ITEM #%d, %d",
                         m_slotsData[trader][trade_slot_no].m_item.m_addInfo,
                         newAddInfo);
                }
            } else {
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                m_slotsData[trader][trade_slot_no].m_field00 = 0;
                m_slotsData[trader][trade_slot_no].m_field04 = 0;
                m_slotsData[trader][trade_slot_no].m_field08 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
                m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
                m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            }
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            otherUser->Send(packet);
            }
            return ret;
        }
    case 3:  // ---- 装备 ----
        {
            if (trade_slot_no == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x59a, "trade_slot_no == 0");
                return -1;
            }
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
            if (copy.m_field08 != 3) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5a2, "moveitem.item_space != ENUM_ITEMSPACE_EQUIPPED");
                return -1;
            }
            if (copy.m_item.m_addInfo == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5a8, "moveitem.item.item_id == 0");
                return -1;
            }
            // ORIG +0x1047 `movzbl 0xa(%eax)`：读**槽**（非 copy）完整寻址
            if (m_slotsData[trader][trade_slot_no].m_item.m_field1
                    == Inven_Item::ITEM_TYPE_AVATAR) {
                if (target_slot > 9)
                    return -1;
            } else {
                if (target_slot <= 9 || target_slot > 0x15)
                    return -1;
            }
            // ORIG +0x109c：成员 sret 形 _ZNK10CInventory12GetInvenSlotEii
            Inven_Item slotItem =
                user->getCurCharacInvenR()->GetInvenSlot(0, target_slot);
            if (slotItem.m_addInfo != 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5bb, "moveitem.item.item_id == 0");
                return -1;
            }
            CEquipItem* equipRef =
                (CEquipItem*)G_CDataManager()->find_item(
                    copy.m_item.m_addInfo);
            if (equipRef == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5c3, "equip_ref == NULL");
                return -1;
            }
            // ORIG +0x1142/+0x114f：实参位直调（level 先于 job 求值，R2L），
            // 结果留 callee-saved 寄存器，不命名局部（P36/P23）
            int res = equipRef->check_able_To_equip(
                (char)target_slot, user->get_charac_job(),
                user->get_charac_level());
            if (res != 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5ca, "equip_ref->check_able_To_equip return false res(%d)",
                    res);
                return -1;
            }
            user->getCurCharacInvenW()->m_invenItem[target_slot] =
                copy.m_item;
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
            ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
            ((InterfacePacketBuf*)&packet)->put_int(-1);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            otherUser->Send(packet);
            m_slotsData[trader][trade_slot_no].m_field00 = 0;
            m_slotsData[trader][trade_slot_no].m_field04 = 0;
            m_slotsData[trader][trade_slot_no].m_field08 = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
            m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
            m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
            m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            m_nState[trader] = 0;
            m_nState[other] = 0;
            if (trade_slot_no <= 0xb) {
                // ORIG +0x14f1/+0x1501：get_state 两次独立调用（P48）
                if (user->get_state() != 5 && user->get_state() != 8) {
                    ((InterfacePacketBuf*)&packet)->clear();
                    ((InterfacePacketBuf*)&packet)->put_header(0, 2);
                    ((InterfacePacketBuf*)&packet)->put_byte(0);
                    ((InterfacePacketBuf*)&packet)->put_short(1);
                    user->make_basic_info((char*)&packet, 0);
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    G_GameWorld()->send_all(packet, user);
                }
            }
            return target_slot;
        }
    case 1:  // ---- 头像 ----
        // ORIG 物理块序：case0(+0x1c4) → case3(+0xf27) → case1(+0x15bb) →
        // case7(+0x186e)（switch 二分树 je 落点决定），源码书写序须一致。
        {
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
            if (copy.m_field08 != 1)
                return -1;
            int ret = m_pTraders[trader]->getCurCharacInvenW()
                          ->insertAvatarIntoSpecificSlot(
                              copy.m_item, target_slot,
                              (eAvatarItemAddReason)4, 1);
            // ORIG +0x16ed `js 尾块`：负值跳 return ret，成功直落
            if (ret >= 0) {
                m_slotsData[trader][trade_slot_no].Reset();
                if (copy.m_field00 != ret) {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->SendAvatarEvent(0x32, copy.m_field00 + 0xa,
                                          ret + 0xa, copy.m_item.m_addInfo2,
                                          0);
                }
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
            }
            return ret;
        }
    case 7:  // ---- 生物 ----
        {
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
            if (copy.m_field08 != 7)
                return -1;
            if (copy.m_item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP) {
                if (!m_pTraders[trader]->getCurCharacInvenW()
                         ->GetCreatureMgrW()
                         ->IsCreatureEquipmentScope(target_slot))
                    return -1;
            }
            int ret = m_pTraders[trader]->getCurCharacInvenW()
                          ->insertCreatureIntoSpecificSlot(
                              copy.m_item, target_slot,
                              (eItemAddReason)0x1d, 1);
            // ORIG +0x19e7 `js 尾块`：负值跳 return ret，成功直落
            if (ret >= 0) {
                m_slotsData[trader][trade_slot_no].Reset();
                // ORIG +0x1a1c：je→WriteLog 落块尾(+0x1a60)，ChangeSlotNo 直落
                if (copy.m_field00 != ret) {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->GetCreatureMgrW()
                        ->ChangeSlotNo(&copy.m_item, ret, 0x1d);
                }
                else {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->WriteCreatureLog(
                            (INVEN_TYPE)3, copy.m_item.m_addInfo,
                            copy.m_item.m_addInfo2, 1, copy.m_item.m_field1,
                            (eItemAddReason)0x1d);
                }
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
            }
            return ret;
        }
    default:
        return -1;
    }
