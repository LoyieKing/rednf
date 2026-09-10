bool CTradeSpace::checkCancelTrade()
{
    // b28f-SweepCTSpace: NEAR（533/533）。ORIG @0x0852dbb8。
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x674, 5)(
            "CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR()"
            " || m_pTraders[1]->getCurCharacR()");
        return false;
    }
    CInventory copy[2];
    // ORIG +0xe4-0x120：mem 构造与 Set 交替（非并列声明后统一 Set）
    InventoryMemory mem0;
    copy[0].SetInventoryMemory(&mem0);
    InventoryMemory mem1;
    copy[1].SetInventoryMemory(&mem1);
    copy[0].setCopy(*m_pTraders[0]->getCurCharacInvenRefR());
    copy[1].setCopy(*m_pTraders[1]->getCurCharacInvenRefR());
    // b28d-solo-CTradeSpace：局部集合照 ORIG 槽序（P212 全头部声明）：
    //   count(-0x30) trader(-0x2c) ret(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)；
    //   got 是 gain_money 返回值（ORIG +0x253 死写入照抄，P171）；两循环变量独立。
    int count = 0;
    int trader = 0;
    int ret;
    int s1;
    int got;
    int s2;
    for (trader = 0; trader <= 1; ++trader) {
        count = 0;
        for (s1 = 3; s1 <= 0x1a; ++s1) {
            if (m_slotsData[trader][s1].m_item.m_addInfo != 0)
                ++count;
        }
        // ORIG +0x20e：嵌套两 if（逗号/&& 混写会诱发 bool 物化岛）
        if (m_nMoney[trader] > 0) {
            ++count;
            if (m_pTraders[trader] != 0)
                got = copy[trader].gain_money(m_nMoney[trader],
                                              (eMoneyAddReason)2, 0, 0);
        }
        for (s2 = 3; s2 <= 0x1a; ++s2) {
            // P223：ORIG 两守卫分落独立 nop 岛（+0x756/+0x759，中间 jmp）
            // ⇒ 负条件 goto 到两个尾标签（标签序 = 守卫序）。
            if (m_slotsData[trader][s2].m_item.m_addInfo == 0)
                goto L_s2_skip1;
            if (m_pTraders[trader] == 0)
                goto L_s2_skip2;
            {
                if (m_slotsData[trader][s2].m_field08 == 0) {
                    ret = copy[trader].insertItemIntoInventory(
                        m_slotsData[trader][s2].m_item, (eItemAddReason)2, 0,
                        1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6af, 5)(
                            "try insert_item(INVEN_TYPE_INVENTORY) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else if (m_slotsData[trader][s2].m_field08 == 1) {
                    ret = copy[trader].insertAvatarIntoInventory(
                        m_slotsData[trader][s2].m_item,
                        (eAvatarItemAddReason)3, 0);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6b9, 5)(
                            "try insert_item(ENUM_ITEMSPACE_AVATAR) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else if (m_slotsData[trader][s2].m_field08 == 7) {
                    ret = copy[trader].insertItemIntoCreature(
                        m_slotsData[trader][s2].m_item, (eItemAddReason)0x1c,
                        0, 1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6c3, 5)(
                            "try insert_item(ENUM_ITEMSPACE_CREATURE) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else {
                    ret = copy[trader].insertItemIntoEquipment(
                        m_slotsData[trader][s2].m_item, (eItemAddReason)2, 0,
                        1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6d8, 5)(
                            "try insert_item(INVEN_TYPE_EQUIP) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                }
            }
        L_s2_skip1:;
        L_s2_skip2:;
        }
    }
    return true;
