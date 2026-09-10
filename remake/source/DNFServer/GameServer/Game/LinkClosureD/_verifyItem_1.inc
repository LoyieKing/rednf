int _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
                bool b1, bool b2, CUser* user)
{
    if (item->m_addInfo == 0)
    {
        // 空槽：必须全零（recovery 校验），残留 → 0x1562
        if (item->m_field0 != 0 || item->m_field1 != 0 ||
            item->GetItemAttr() != 0 || item->m_addInfo2 != 0 ||
            item->m_fieldb != 0 || item->m_fieldd != 0 ||
            *(int*)(reinterpret_cast<const char*>(item) + 0x11) != 0)
            return 0x1562;
        return 0;
    }

    if (item->m_addInfo == 0x71d7)
    {
        // GUID 物品：渲染 guid 串并 trace（校验继续）。ORIG 0x084efd9b..efe0c：
        // guid→%edi、characNo→%esi、accId→%ebx 均在 cMyTrace ctor 之前求值并跨
        // ctor 保活 ⇒ P227 全表达式临时链（具名局部会各自占槽，帧 +0x14）。
        CItemGloballyUniqueIdentifierPrint print;
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x156a, 5)("[HaveAccountLog] Invalid item. (mid:%u, charac:%u, item:%u, guid:%s)",
              user->get_acc_id(), user->get_charac_no(-1),
              (unsigned int)item->m_addInfo,
              print(*(const itemGloballyUniqueIdentifier_t*)(
                  const void*)(reinterpret_cast<const char*>(item) + 0x15)));
    }

    const CItem* def = G_CDataManager()->find_item(item->m_addInfo);
    if (def == 0)
        return 0x1576;
    if (item->m_addInfo != def->get_index())
        return 0x157a;
    if (item->m_field0 != 0 && b1)
        return 0x157d;

    if (def->is_stackable())
    {
        if (b2)
            return 0x1582;
        // ORIG 0x84efec2：t 全程寄存器（无槽，P0）⇒ 内联表达式；位测写
        // `(1u<<t) & 0x69c`（左操作数先物化，复现 ORIG shl 形；0x69c 在右
        // 会触发 4.4 强度归约 shr/and $1，N6/N86b）。t>0xa 检查先于位测。
        register unsigned int t = (unsigned char)item->m_field1;
        if (t > 0xa || ((1u << t) & 0x69c) == 0)
            return 0x158f;
        // ORIG 0x84efef8 直接以 def 槽为 this（无 stackDef 具名局部）
        if (item->m_addInfo2 >
                static_cast<const CStackableItem*>(def)->getStackableLimit() ||
            item->m_addInfo2 < 0)
            return 0x159e;
    }
    else
    {
        // 非 stackable：ORIG 0x84eff2f..ff4f 为二分决策树（cmp $6 jg /
        // cmp $5 jge / cmp $1 je / cmp $8 je）⇒ 原码是 switch（数值升序 case）。
        switch ((int)(unsigned char)item->m_field1)
        {
        case 1:
            break;
        case 5:
        case 6:
            if (type != INVENTORY_TYPE_0 && type != INVENTORY_TYPE_3)
                return 0x15b0;
            break;
        case 8:
            if (type != INVENTORY_TYPE_0)
                return 0x15b4;
            break;
        default:
            return 0x15b7;
        }
    }

    if (def->get_rarity() <= 1 && item->GetReSealCount() != 0)
        return 0x15c2;

    // p167：ORIG 只 call 一次 GetAttachType（结果留 %eax 供 cmp+shl）⇒ 单次求值，
    // 纯 register 免槽（P231）。
    register unsigned int at = (unsigned int)def->GetAttachType();
    if (at <= 5)
    {
        if ((0x27 & (1u << at)) != 0)
        {
            if (item->GetReSealCount() != 0)
                return 0x15d9;
            if (item->m_field0 != 0)
                return 0x15e1;
        }
    }

    if (type != INVENTORY_TYPE_1)
        return 0;

    if (user->getCurCharacR() == 0)
        return 0x15f0;

    int slotType = user->getCurCharacInvenR()->GetSlotItemType(slot);
    if (slotType != 0)
    {
        if ((unsigned int)item->m_field1 != (unsigned int)slotType)
            return 0x15f8;
    }
    return 0;
