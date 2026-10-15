// ORIG execute 0x084ac2e2：合法物品以系统邮件发放（0 号特例 count=1，1 号特例
// 金币条目手工填充，其余走 CItem::make_item 虚拷贝并附加 add_info=1）。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084ac2e2 orig/ours=187/132
//   残差：EXTRA memset ×2（我方数组零初始化 call vs ORIG 内联 rep stos，N78）
//   + 槽/寄存器分配跨版本差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=9 [N7/N18a] p167 CLEAR(16/16)；仅剩
// 寄存器角色差（ORIG `mov -0x28,%ebx` vs 4.4 `mov -0x28,%edx`）与虚调用 receiver
// 装载序（N7 族），语义完成封顶。
// [b28d-solo-2] verdict=DIFF：真缺口 = itemId 预清零缺失。ORIG +0x03e 在数组清零
// **之前**发 `movl $0,-0x28`（itemId 预清）、+0x16f 才写 m_itemIdx ⇒ 补两句式
// `int itemId = 0; … itemId = m_itemIdx;`（P198①）后 orig/ours=187/187。
// 残余 2 条：`G_CDataManager()->find_item(itemId)` ORIG 先装实参到 %ebx 再调
// receiver，4.4 恒 receiver 先（P75/P23 跨版本求值序，探针 p2/p3 五形不翻）。
//   [C4A0Nine 2026-09-11 复验] 第 6 形 deEv（`(*G_CDataManager()).find_item(itemId)`）
//   亦不翻（187/187 不变；ORIG 0x084ac4b1 `mov -0x28(%ebp),%ebx` 跨 receiver call
//   保活为 4.1 独有形）⇒ P75/P23 封顶不变。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; lea -0x82(%ebp),%eax;
//   mov %eax,0x4(%esp); mov -0x1c(%ebp),%eax; mov %eax,(%esp); call *%edx` vs 4.4.7 用 %eax 目标；
//   另 `mov -0x28(%ebp),%ebx` 保活（ORIG）vs 4.4.7 直读（P226/N18a）。
// [round5-A20 复核·N126] fresh .o 185/185、判据⑪ PURE SHIFT c=0x0（frame 428=428、
//   槽集全等）⇒ 残差纯为 N7 间接调用 receiver 寄存器角色（ORIG `mov (%eax),%edx;
//   …call *%edx` vs 4.4.7 全 %eax）+ `mov -0x28(%ebp),%ebx` 保活（P226/N18a）。
//   P230 实测 4.4.6 后端亦不达 ⇒ 属路由域，封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 两串 `"game_server_msg_144"`
//   （0x084ac3da）/`"game_server_msg_143"`（0x084ac418）与源码 findString(4,…) 逐条一致；
//   §6.5.1 enc 0 命中。
void CAddItemByMail::execute()
{
    CUser* user = GetUser();
    // [b28d-solo-GmCmds_A] ORIG +0x02c 把 find_item 结果存入具名槽 -0x2c
    // （`mov %eax,-0x2c; cmpl $0,-0x2c; je`），内联 test/sete 形不符；
    // 实参用 m_itemIdx(+8) 直读（无 itemId 槽，其槽 -0x28 在两次 strncpy 后写）。
    CItem* found = G_CDataManager()->find_item(m_itemIdx);
    if (found == 0)
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P312：ORIG 对 title[0x100](-0x182)/sender[0x15](-0x45)
    // 发内联对齐剥离清零（无 memset call，p167 EXTRA memset×2）。
    // [b28d-solo-2] P198①：ORIG +0x03e 在数组清零**之前**发 `movl $0,-0x28`
    // （itemId 预清），+0x16f 才写 m_itemIdx ⇒ 两句式（声明=0 + 后赋值）。
    int itemId = 0;
    char title[0x100] = {0};
    char sender[0x15] = {0};
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_144", 0),
            0xff);
    strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_143", 0),
            0x14);

    // [b28d-solo-GmCmds_A] ORIG 声明/初始化序：itemId(-0x28) → addInfo(-0x24)
    // → Inven_Item C1(-0x82) → count(-0x20)。
    itemId = m_itemIdx;
    int addInfo = 1;
    Inven_Item item;
    int count = 0;
    if (itemId == 0)
    {
        count = addInfo;
    }
    else if (itemId == 1)
    {
        item.m_addInfo = 1;          // +0x02
        item.m_itemType = 2;           // +0x01
        item.m_addInfo2 = addInfo;   // +0x07
        item.m_endurance = 1;           // +0x0b
        item.ResetItemAttr();
    }
    else
    {
        CItem* proto = G_CDataManager()->find_item(itemId);
        if (proto == 0)
        {
            return;
        }
        item.m_addInfo = itemId;     // +0x02
        proto->make_item(item);      // ORIG vtbl[2]
        item.set_add_info(addInfo);
    }

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, static_cast<unsigned int>(count),
        static_cast<unsigned int>(user->getCurCharacNo()), title,
        static_cast<int>(strlen(title)), 0u, user->GetServerGroup(), false, false);
}