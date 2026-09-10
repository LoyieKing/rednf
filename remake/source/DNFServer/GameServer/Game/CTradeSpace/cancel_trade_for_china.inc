bool CTradeSpace::cancel_trade_for_china(CUser* user)
{
    // b28d-35: DIFF→N82（798/814）。ORIG @0x0852e544。
    PacketGuard packet[2];                      // -0x58
    int user_index = 0;                         // -0x2c（ORIG 仅循环前 +0x0c/0x13 置零）
    int count;                                  // -0x30（每外圈头 +0x88 置 0）
    int done;                                   // -0x28（循环内 +0x117 置 0）
    int countIdx;                               // -0x5c（循环内 +0x11e 置 0）
    int ret;                                    // -0x24（循环内 +0x2f9 置 0）
    for (; user_index <= 1; ++user_index) {
        count = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[user_index][s].m_item.m_addInfo != 0)
                ++count;
        }
        ((InterfacePacketBuf*)&packet[user_index])->clear();
        ((InterfacePacketBuf*)&packet[user_index])->put_header(0, 0x10);
        done = 0;
        countIdx = 0;
        // ORIG +0x136：money<=0 跳 else（仅占位 put_short(count)）
        if (m_nMoney[user_index] > 0) {
            ++count;
            if (m_pTraders[user_index] != 0) {
                if (m_pTraders[user_index]->getCurCharacR() == 0) {
                    cMyTrace(__PRETTY_FUNCTION__, 0x71c, 5)(
                        "CTradeSpace::cancel_trade()\t"
                        "m_pTraders[user_index]->getCurCharacR()");
                    return false;
                }
                m_pTraders[user_index]->getCurCharacInvenW()->gain_money(
                    m_nMoney[user_index], (eMoneyAddReason)2, 1, 0);
                m_nMoney[user_index] = 0;
                ++done;  // ORIG +0x20b：金币条目计入实际数
            }
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
        } else {
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
        }
        for (int s = 3; s <= 0x1a; ++s) {
            // ORIG +0x30c：owner、getCurCharacR、槽非空三连（每次重读）
            if (m_pTraders[user_index] != 0 &&
                m_pTraders[user_index]->getCurCharacR() != 0 &&
                m_slotsData[user_index][s].m_item.m_addInfo != 0) {
                ((InterfacePacketBuf*)&packet[user_index])->put_short(s);
                if (m_slotsData[user_index][s].m_field08 == 1) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertAvatarIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eAvatarItemAddReason)3, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenR()
                            ->SendAvatarEvent(
                                0x32,
                                m_slotsData[user_index][s].m_field00 + 10,
                                ret + 10,
                                m_slotsData[user_index][s].m_item.m_addInfo2,
                                0);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(1);
                } else if (m_slotsData[user_index][s].m_field08 == 7) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoCreature(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)0x1c, 1, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->ChangeSlotNo(
                                &m_slotsData[user_index][s].m_item, ret,
                                0x1c);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(7);
                } else if (m_slotsData[user_index][s].m_field08 == 0) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
                } else {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoEquipment(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(3);
                }
                // ORIG +0x863：插入失败 → 邮件补偿 + 槽 item reset + 回写 ret
                if (ret < 0)
                    send_lose_item_for_china(
                        m_pTraders[user_index],
                        m_slotsData[user_index][s].m_item);
                m_slotsData[user_index][s].m_item.reset();
                ((InterfacePacketBuf*)&packet[user_index])->put_short(ret);
                ++done;
            }
        }
        // ORIG +0x936：以实际条目数回填 count 占位（put_short(int&,int)）
        packet[user_index].put_short(countIdx, done);
        packet[user_index].finalize(true);
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0 && m_pTraders[i] != user) {
            m_pTraders[i]->Send(packet[i]);
            m_pTraders[i]->send_equip(0);
        }
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0)
            m_pTraders[i]->m_historyLog.TradeEnd((eTradeEndReason)0, 0, 0, 0,
                                                 0);
    }
    return true;
