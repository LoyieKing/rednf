// ---- checkCancelTrade @ 0x0852dbb8 ----
// 残差豁免（O=533/N=528）：ORIG 空 else 的 nop;jmp 对齐岛（+0x756 等，
// nop-strip 后消）+ Set 实参 lea 位差。
// 校验取消交易可行（ORIG 0x852dbb8，O=533）：双方各造一个 CInventory 副本
//（SetInventoryMemory 挂独立 InventoryMemory + setCopy(getCurCharacInvenRefR)），
// 模拟把各交易者自己登记的物品按 m_field08（0=背包/1=头像/7=宠物/else=装备）
// 与金钱插回其自身副本；任一插入失败 → 该分支专属 trace 并返回 false。
// 形态要点（逐点对 ORIG 汇编）：
//   - 双 mem 独立对象（ctor→Set 交替，+0xdc..0x120 序列），非 char[]+memset
//   - count(local_34) 跨外圈共享；money 分支 ++count 位于 && 短路内
//   - item 循环条件：槽非空在前、owner 判空在后（+0x23f-0x253）
//   - 四分支失败 trace 各独立字符串（0x6af/0x6b9/0x6c3/0x6d8），
//     characNo 先于 trace 构造求值（链式，P37）
// §6.2 巨件核销：P167 35/35 [0缺0多]；gap 5→4；真修：P191 trader 双初始化
//   （int trader=0; + for(trader=0;…) 复现 ORIG +0x189/+0x190 双 movl，O533/N529）。
//   残差 4 条归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [dead-store] +0x253 gain_money 返回值 mov %eax,-0x20 死存，4.4 对未读
//     局部恒不占槽（技巧31 负结论），源码不可复现 +1；
//   [跨版本空体岛] +0x756/+0x759 两条失败路径各挂 nop;jmp;nop 中继
//     （负例表「空 else nop;jmp 岛 vs jump-threading」行），4.4 恒直达自增 +3。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=17（O531/N530）]
//   修法：局部全头部声明照 ORIG 槽序（P212）：count(-0x30) trader(-0x2c)
//   ret(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)；补 gain_money 返回值死写入
//   got（P171 照抄）；两循环变量独立；trader 双初始化（P191）。173→17。
//   残差：①InventoryMemory 栈地址差 1/2 字节（同 checkTrade，Main 裁决 N）；
//   ②尾部一条 jmp 中继（N121/epilogue-threading）。p167 47/47 CLEAR。
// [b28f-SweepCTSpace verdict=NEAR ndiff=186（O533/N533, gap 0）——推翻旧 N 判词]
//   真修：P223 双岛形。ORIG +0x283/+0x29c 两条守卫各自 `je` 分落独立 nop 岛
//   （+0x756 nop / +0x757 jmp / +0x759 nop），4.4 对 `if(A && B)` 恒合并共享
//   出口 ⇒ 改写「负条件 goto + 尾标签串」：
//     if (m_addInfo == 0) goto L_s2_skip1; if (m_pTraders[trader] == 0) goto
//     L_s2_skip2; { body } L_s2_skip1:; L_s2_skip2:;
//   O530→533（=ORIG）、DIFF→NEAR（四数 near+1/diff-1）、ndiff 187→186，
//   逐 hunk 净差全 0（残差仅 InventoryMemory 栈地址 -0xd12f↔-0xd130 等
//   1~2 字节布局差，Main 已裁决 N）。p167 47/47 CLEAR。
//   [K2 2026-09-11 复验] fresh .o：残差 4 行 = -0xd12f/-0x1926e ↔ -0xd130/-0x19270
//   （1B/2B，同 checkTrade 的 N115 族：4.1 对 0xC13F 尺寸对象不补齐 4 对齐）；
//   指令数 533=533、p167 35/35（仅 C1/C2·D1/D2 别名差 P93）⇒ 维持 NEAR 封顶。
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
}