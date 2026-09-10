int CInventory::stackInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 192 / 我 192（gap 0，p167 仅 cMyTrace C1/C2 同址别名 P93）。
    // 真修：日志 cMyTrace 由具名两语句改**全表达式临时链**（P227/P37）⇒ 4.4 在 ctor 前
    // 右起求值全部实参并跨 ctor 以 esi/edi/ebx 保活，与 ORIG +0x0a4..+0x128 逐段同形。
    // 旧判词「残余 3 = N115 帧差」被推翻（帧/push 已全等，仅尾 nop 对齐差豁免）。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                // [b28d-round5 复攻失败记录] 把这两个具名局部提升到函数头（P212 全头部声明）
                // 实测槽集**零变化**（OURS-only=[-0x34,-0x24,-0x20] 原样）⇒ 已回滚；
                // 残余槽序属 N82附（4.4 聚合优先低槽 + 标量分区）封顶。
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                cMyTrace(__PRETTY_FUNCTION__,
                         0x157c, 4)
                    ("INVEN INT_MAX setting 1 : m_id(%s),charac_id(%d) inven(%d)+add(%d) item_id(%d)",
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
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.m_addInfo2);
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
                        0x1597, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
        }
        return i;
    }
    return -2;
