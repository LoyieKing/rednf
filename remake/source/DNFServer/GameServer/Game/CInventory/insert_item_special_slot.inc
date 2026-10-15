int CInventory::insert_item_special_slot(Inven_Item item, int a, int b,
                                         eMoneyAddReason moneyReason, eItemAddReason itemReason)
{
    int i;   // ORIG 单一 -0x1c 槽：两处循环共用同一 i（4.1 不复用槽，4.4 分声明会拆两槽）
    // b28d-solo-CInventory：429/371，P167 EXTRA setCopy×2（ORIG 内联拷贝）⇒ N82/N84 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差；P167 EXTRA `Inven_Item::setCopy`×2 系 ORIG 内联拷贝形差异（无 MISSING）；429/371。
    if (item.m_addInfo == 0)
    {
        if (a < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1742, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x1742);
            return -2;
        }
        gain_money(a, moneyReason, true, 0);
        return 0;
    }
    if (item.m_itemType == 1)
    {
        // ORIG -0x1c 槽即函数级索引 i：b 分支以 b 直读（余 1 行为 b(0x50)↔i(-0x1c)
        // 的装载选择，4.4 读形参 / 4.1 读具名槽；加 i=b 承接会 +2 条，反劣 ⇒ 保留 b 形）。
        // [b28d-36 复测] 全 b→i 替换（i=b; m_pEquipSlot[i]…return i）实测 nd 1→391 大幅反劣，
        // 已回滚 ⇒ 确认保留 b 直读形，残余 1 条记 4.4 形参直读 vs 4.1 具名槽（N 类微差）。
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b] = item;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, 1, 1, item, itemReason);
            }
            return b;
        }
    }
    else
    {
        for (i = 3; i < 9; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    if (m_pParent != 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x177c, 4)
                            ("INVEN INT_MAX setting 3 : mid(%d),characNo(%d),inven(%d)+add(%d) item_id(%d)",
                             NumberToString(m_pParent->get_acc_id(), 0),
                             m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a,
                             item.m_addInfo);
                    }
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        int start;
        int end;
        StartEndPos((Inven_Item::ITEM_TYPE)item.m_itemType, start, end);
        for (i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                    if (m_pParent != 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x179f, 4)
                            ("INT_MAX setting 3 : mid(%d),characNo(%d), inven(%d)+add(%d)",
                             NumberToString(m_pParent->get_acc_id(), 0),
                             m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a);
                    }
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b] = item;
            if (m_pParent != 0)
            {
                // ORIG 0x08503fb8：`mov -0x1c(%ebp),%eax` + imul $0x3d ⇒
                // get_add_info 的 receiver 是**具名局部 i**（循环退出值），
                // 而 `return b`（0x08503ffa `mov 0x50(%ebp),%eax`）用形参 b。
                // 原码此处混用 i/b（P171 照抄），改动仅此一处 receiver。
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(), item,
                    itemReason);
            }
            return b;
        }
    }
    return -1;
}