// ---- checkTrade @ 0x08530210 ----
// 残差豁免（O=425/N=424）：SetInventoryMemory 实参 lea 装载位差 1 条。
// 校验交易可完成（ORIG 0x8530210，O=425）：双方各造 CInventory 副本
//（setCopy 直取 characR()->m_inven，+0xda 成员地址，无 InvenR getter），
// 模拟把 trader 登记的物品/金钱插入**对方** copy[1-trader]；
// 金钱分支 gain_money 返回值 != 提交额即失败；物品三分支
//（item.m_field1==8 头像 / m_field08==7 宠物 / else 背包），失败按
// field1==8 双 trace（0xba7/0xbab）。bool 返回（PRETTY @0x8c98444）。
// [b28d-solo-CTradeSpace verdict=NEAR ndiff=16（O425/N425）]
//   修法：局部全头部声明照 ORIG 槽序（P212）：count(-0x30) ret(-0x2c)
//   trader(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)，两循环变量物理独立
//   （4.4 复用槽少一槽）；trader 单初始化（`int trader=0; for(;…)`）。
//   109→16。残差 16 = InventoryMemory 两局部栈地址差 1/2 字节
//   （ORIG -0xd0ff/-0x1923e vs 我方 -0xd100/-0x19240）。Main 2026-09-09 裁决：
//   不得删 InventoryMemory::m_tailC13f（ORIG 全量 110 处 CUser+0x858d4 反证
//   sizeof=0xC140），本残差按 N 封顶（4.1 相邻同型局部 1B 布局差）。
//   [K2 2026-09-11 复验] fresh .o 复核残差仍仅 4 行 = 两组 `lea -0xd0ff/-0x1923e`
//   vs `-0xd100/-0x19240`（差 1B/2B）；两对象间隔 ORIG 0xC13F = InventoryMemory
//   四段 memset 之和（CUser/reset_4.inc 0x4a58+0x1905+0x39aa+0x2438）⇒ 归
//   **N115**「4.1 栈分配非 4 对齐 vs 4.4 恒 4 对齐」，同族判例
//   WongWorkMiscB_A.cpp:415（-0xca0b↔-0xca0c）与 ExpertExtraction.cpp:168（-0xc957↔-0xc958）。
//   非 CInventory sizeof 问题（sizeof 双侧一致 0xC140，Main 已裁）。
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
}