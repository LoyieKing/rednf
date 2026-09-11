// ===========================================================================
// 3) CUser::Redeem_Item(int) —— ORIG T 0x08646df2
//    报告 CUser/Redeem_Item.md 全文转写。
// [SIGFIX:CUser::Redeem_Item] ORIG __PRETTY_FUNCTION__ 串为 bool（Main 117 件
// 清单）；返回值仅 0/1，CUser.h:3537 已声明 bool ⇒ 定义同步（mangled 不含返回类型）。
// ===========================================================================
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool CUser::Redeem_Item(int redeemId)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：301/293、p167 CLEAR(44=44)；
    // 残差=①4.4 局部槽基整体深 4（redeem -0x1c vs -0x18、info -0xd0 vs -0xcc；
    // 引用改指针无效）⇒ N95/N82；②4.4 位折叠 `(unsigned)price>>31` 折成 cmpl/jns
    // （ORIG `shr $0x1f; test %al,%al`）⇒ N6/N99。P198① 预置 -1 与 P221 trace 临时链已复现。
    // GetCharacExpandData(6) 即 Redeem_Item::CRedeemItem（槽位 6，CUser.cpp 同型 cast）
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
    {
        // [b28d-solo-UserMiscLink] P221/P227：两个互斥分支的 cMyTrace 改全表达式临时链，
        // 4.4 具名对象恒「先声明深→浅」与 ORIG（-0x7c/-0x6c）相反。
        cMyTrace(__PRETTY_FUNCTION__, 0x529, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CheckInTrade())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x52f, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    // [b28d-solo-UserMiscLink] ORIG 0x08646f97：`movl $0xffffffff,-0x14` 预置 +
    // 独立赋值（P198①）；负值判定为 `shr $0x1f,%eax; test %al,%al; je`（4.1 无符号移位形）。
    int price = -1;
    price = redeem->GetItemPrice(redeemId);
    // [L4b 试验] 4.4 对 `(unsigned)price >> 31 != 0` 恒折叠为 `cmpl $0; jns`
    // （ORIG 4.1 为 `shr $0x1f,%eax; test %al,%al; je`）。显式 8 位窄化比较
    // 以期 4.4 直接产 `test %al,%al`（不引入槽往返）。
    if ((unsigned char)((unsigned int)price >> 31) != 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CUserCharacInfo::getCurCharacMoney() < price)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 0x46);
        return 0;
    }

    Redeem_Item::Inven_Item_Expand_RedeemInfo info = redeem->GetRedeemInfoR(redeemId);
    // info 前 0x3d 字节即 Inven_Item 快照（AddRedeemList 复制口径）；m_item 为
    // 真实 Inven_Item 成员（pack(1) 布局一致；ORIG 以具名指针 local_14(-0x10) 消费，
    // 引用形会多占一槽使全帧深 4）。
    Inven_Item* item = reinterpret_cast<Inven_Item*>(&info.m_item);
    if (item->isEmpty())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    int slot = CUserCharacInfo::getCurCharacInvenW()->insertItemIntoInventory(
        *item, (eItemAddReason)0x29, true, true);
    if (slot < 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 4);
        return 0;
    }

    // [L4b 语义轴③ 字段选择] ORIG 0x08647146 `mov -0x8c(%ebp),%ebx`（该函数内
    // -0x8c 全函数**只读不写**，即 info 起始 -0xcc 的 +0x40 = `m_count40`；
    // GetRedeemInfoR 的 sret 写入 0x44 字节覆盖该槽）⇒ use_money 的 amount 实参
    // 是 info.m_count40（正常路径与 price=GetItemPrice 同值，均取该 list 项
    // 的 +0x40），非 price 局部（旧形产 `mov -0x14(%ebp),%edx` 与本形不符）。
    CUserCharacInfo::getCurCharacInvenW()->use_money(info.m_count40,
                                                     (eMoneySubReason)0x25, true);

    // 非堆叠 → 历史记录 count=1；堆叠 → count=item.m_addInfo2(+0x07)。
    // [L4b 语义轴① 去发明防御] ORIG 0x0864718c `mov %eax,(%esp); call is_stackable`
    // 把 find_item 返回值**直接**当 this 传入，**无** null 检查（无 test/je 岛）；
    // `pItem == 0 ||` 属我方发明的防御，与 ORIG 行为不符 ⇒ 删除（ORIG 同址即
    // 0x08647194 `test %al,%al; je +0x3cd` 直判 is_stackable 极性）。
    // [L4b P59/P222 块序] ORIG：is_stackable **真** → fall-through 走 addInfo2 分支
    // （0x08647198..0x086471b8），**假** → je +0x3cd 走 count=1 ⇒ 正极性书写。
    // [L4b P0 去具名局部] ORIG 0x0864718c 把 find_item 返回值经 %eax 直传
    // is_stackable，无 pItem 局部槽（我方旧形的 `mov %eax,-0xc; mov -0xc,%eax`
    // 往返为多余）⇒ 表达式内联。
    if (G_CDataManager()->find_item(item->m_addInfo)->is_stackable())
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, item->m_addInfo2);
    }
    else
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, 1);
    }

    redeem->RedeemItem(redeemId);

    PacketGuard packet;
    packet.put_header(1, 0x137);
    packet.put_byte(1);
    packet.put_int(slot);
    packet.finalize(true);
    Send(packet);
    return 1;
}