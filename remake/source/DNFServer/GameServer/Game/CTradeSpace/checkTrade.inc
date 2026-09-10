bool CTradeSpace::checkTrade()
{
    // b28d-35: DIFF→N82（425/424 差 1 条；§6.2 口径）。ORIG @0x08530210。
    CInventory copy[2];
    // ORIG +0xe4-0x120：mem 构造与 Set 交替（非并列声明后统一 Set）
    InventoryMemory mem0;
    copy[0].SetInventoryMemory(&mem0);
    InventoryMemory mem1;
    copy[1].SetInventoryMemory(&mem1);
    // CUser.h _Charac_info+0xda 内嵌 CInventory（CUserCharacInfo 头同区 m_inven 视图）；
    // ORIG 0x85302f9 直取 characR()+0xda 传 setCopy。
    // [Lane37CUser 2026-09-10] pad `m_padda` 已成员化为 `m_curCharacInven`（同址 +0xda，
    //   static_assert 锁定）⇒ 直传具名成员，免 cast。
    copy[0].setCopy(m_pTraders[0]->getCurCharacR()->m_curCharacInven);
    copy[1].setCopy(m_pTraders[1]->getCurCharacR()->m_curCharacInven);
    // b28d-solo-CTradeSpace：局部集合照 ORIG 槽序（P212 全头部声明）：
    //   count(-0x30) ret(-0x2c) trader(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)，
    //   两循环变量物理独立（4.4 复用槽会少一槽，ORIG 4.1 不复用）。
    int count = 0;
    int ret = 0;
    int trader = 0;
    int s1;
    int got;
    int s2;
    for (; trader <= 1; ++trader) {
        count = 0;
        for (s1 = 3; s1 <= 0x1a; ++s1) {
            if (m_slotsData[trader][s1].m_item.m_addInfo != 0)
                ++count;
        }
        // ORIG +0x194：money>0 → ++count → m_pTraders[1-trader] 判空 →
        // gain_money 到对方副本；返回值 != 提交额 → false
        if (m_nMoney[trader] > 0) {
            ++count;
            if (m_pTraders[1 - trader] != 0) {
                got = copy[1 - trader].gain_money(
                    m_nMoney[trader], (eMoneyAddReason)1, 0, 0);
                if (m_nMoney[trader] != got)
                    return false;
            }
        }
        for (s2 = 3; s2 <= 0x1a; ++s2) {
            // 条件序：对方指针在前、槽非空在后（ORIG +0x22d-0x267）
            if (m_pTraders[1 - trader] != 0 &&
                m_slotsData[trader][s2].m_item.m_addInfo != 0) {
                if (m_slotsData[trader][s2].m_item.m_field1 ==
                    Inven_Item::ITEM_TYPE_AVATAR) {
                    ret = copy[1 - trader].insertAvatarIntoInventory(
                        m_slotsData[trader][s2].m_item,
                        (eAvatarItemAddReason)1, 0);
                } else if (m_slotsData[trader][s2].m_field08 == 7) {
                    ret = copy[1 - trader].insertItemIntoCreature(
                        m_slotsData[trader][s2].m_item, (eItemAddReason)0x1a,
                        0, 1);
                } else {
                    ret = copy[1 - trader].insertItemIntoInventory(
                        m_slotsData[trader][s2].m_item, (eItemAddReason)1, 0,
                        1);
                }
                if (ret < 0) {
                    if (m_slotsData[trader][s2].m_item.m_field1 ==
                        Inven_Item::ITEM_TYPE_AVATAR)
                        cMyTrace(__PRETTY_FUNCTION__, 0xba7, 5)(
                            "try insert_item(INVEN_TYPE_AVATAR) fail - "
                            "fatal! ch=%d",
                            m_pTraders[1 - trader]->getCurCharacNo());
                    else
                        cMyTrace(__PRETTY_FUNCTION__, 0xbab, 5)(
                            "try insert_item(INVEN_TYPE_INVENTORY) fail - "
                            "fatal! ch=%d",
                            m_pTraders[1 - trader]->getCurCharacNo());
                    return false;
                }
            }
        }
    }
    return true;
