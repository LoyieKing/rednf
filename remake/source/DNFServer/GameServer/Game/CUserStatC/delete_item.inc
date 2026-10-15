// ============================================================================
// CUser::delete_item(char space, int position, unsigned long itemIdx,
//                    int count, eItemDelReason reason)
// ORIG 0x0866285c sz 0x25d。返回：0 成功 / 0x11 校验或删除失败 / 0x13 交易中。
// [b28d-solo-CUserStatC 判词·AE] P167 CLEAR（17/17），orig 162 = our 162 逐条对齐
//   （仅跳表地址/jump-table 重定位属 AE 口径）。本件修法：
//   ① 0x086628c1 比较：ORIG 为 4.1 bool 寄存器物化 `setne %al; test %al,%al; je`，
//      if 形 4.4 恒折 `je` 直跳 ⇒ 改 P225 形 `while (cond) { return 0x11; }`
//      （探针 /tmp/b28d_cstatc_probe2.cpp fJ 逐条命中）。
//   ② switch 叶块物理序 = case 书写序（P46）：ORIG 序 case0,case2,case3,case1,case7
//      ⇒ 源码 case 1 移至 case 3 之后、case 7 之前（原序 case0,1,2,3,7 致块错位）。
// ============================================================================
int CUser::delete_item(char itemSpace, int position, unsigned long itemIdx,
                       int count, eItemDelReason reason)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N89）。P167 CLEAR（17/17）。@0x866285c
    if (CheckInTrade())
        return 0x13;

    // GetInvenSlot(out, curCharacInvenR(), GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), position)
    Inven_Item slotItem = getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace), position);
    // b28d-solo-CUserStatC：ORIG 0x086628c1 该比较为 4.1 的 bool 寄存器物化
    // （setne %al; test %al,%al; je，无栈槽）；if 形 4.4 恒折 je 直跳。
    // P225 形（while+return）精确复现四段物化（探针 /tmp/b28d_cstatc_probe2.cpp fJ）。
    while ((unsigned long)slotItem.m_addInfo != itemIdx)
    {
        return 0x11;
    }

    switch (itemSpace)
    {
    case 0:  // INVEN_TYPE_EQUIP(1) 背包
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, position, count,
                                               reason, true))
            return 0x11;
        break;
    case 2:  // 角色仓库
        if (!getCurCharacCargoW()->delete_item(position, count, reason))
            return 0x11;
        break;
    case 3:  // INVEN_TYPE_INVEN(0) 装备栏 → 需回发装备
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_INVEN, position, count,
                                               reason, true))
            return 0x11;
        send_equip(position);
        break;
    // b28d-solo-CUserStatC：ORIG 叶块物理序 = case0,case2,case3,case1,case7
    // （P46：叶块排放顺序 = case 书写顺序），故 case 1 紧跟 case 3。
    case 1:  // INVEN_TYPE_AVATAR(2)
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_AVATAR, position, count,
                                               reason, true))
            return 0x11;
        break;
    case 7:  // INVEN_TYPE_CREATURE(3)：宠物装备且类型!=5(ITEM_TYPE_CREATURE_EQUIP)
    {
        const Inven_Item* ref =
            getCurCharacInvenW()->GetInvenRef(INVEN_TYPE_CREATURE, position);
        if (ref == 0)
            return 0x11;
        if (ref->m_itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            return 0x11;
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_CREATURE, position,
                                               count, reason, true))
            return 0x11;
        break;
    }
    default:  // 4/5/6：仅通过索引校验，无动作即成功
        break;
    }
    return 0;
}