// [N82 封顶] batch-10：调用点差集 20=20 归零（set 处 item.get_add_info 补回）；
// 残余 gap=4 = ORIG push/pop %esi/%edi 跨块持值 vs 本工具链栈槽 spill。
int CInventory::stackInvenItemIntoCorrespondingSpace(Inven_Item item, eItemAddReason reason,
                                                     bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 209 / 我 209（gap 0）。同 stackInvenItemIntoQuickSlot：
    // 日志 cMyTrace 改全表达式临时链（P227/P37），实参求值序/跨 ctor 保活与 ORIG 同形。
    // [K8CInventory 2026-09-11 复核] 209/209、结构/调用集/结构偏移直方图全同；残余仅槽号
    // （(8,)×21 / (16,)×3 / (-36,)×4）与 bool 溢出槽差 0x10（ORIG -0x3c vs 我方 -0x4c，
    // 帧 0x5c vs 0x6c）⇒ 与 gain_money 同族（4.4 bool spill 压帧底 + 局部区 rounding），
    // N82附 定谳。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                // [b28d-round5 复攻失败记录] 把 itemAddInfo/itemInfo 提升到函数头（T1b 配方②）
                // 实测 209/209、四数不变、槽集不变 ⇒ 已回滚（ORIG 的深槽 -0x44/-0x40 与 4.4
                // 的浅槽 -0x20/-0x1c 差属 N82附 分区族，非「多开具名局部」）。
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                cMyTrace(__PRETTY_FUNCTION__,
                         0x15b7, 4)
                    ("INVEN INT_MAX setting 2 : m_id(%s),charac_id(%d) inven(%d)+add(%d) item_id(%d)",
                     NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                     m_pEquipSlot[i].get_add_info(), itemInfo, itemAddInfo);
            }
            m_pEquipSlot[i].set_add_info(0);
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info() + item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.get_add_info());
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x15d2, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
        }
        return i;
    }
    return -2;
}