// [N82 封顶] batch-10 续派：差集 39=39 归零；8 处 log 块 parent 判向按 ORIG
// 正向 if(m_pParent!=0){log}else{trace} 翻转后 je/jne 计数逼近（22/8 vs 18/12）。
// 残余 gap=8：ORIG cur-count<0 物化用 add/sub+jle 三 insn 形、cmp/cmpl 与 movl 立即数
// 落槽别名，源层不可控（范式回滚实验见 git 历史），判 N82。
// [b28d-Inventory2 本轮推翻 N82] 625/617 DIFF → 623/623 AE（p167 余 cMyTrace
// C1/C2 同址别名伪差，P93）。三处真因：①外层 if/else 链误译 ⇒ ORIG 0x0850405a
// 是 switch(INVEN_TYPE) 二分决策树（探针实测 4.4.7 同形、叶块序=case 书写序 0,1,2,3，
// P324）；②8 处 cMyTrace 具名局部 ⇒ 全表达式临时链（P221）复现 ORIG 槽阶梯
// -0x88..-0x18；③两处 `cur-count<=0` 早返回 ⇒ 正向 if/else（ORIG `sub;test;jle
// <log 块>`）；另 (INVEN_TYPE)1/0/3 硬编码改回 invenType 形参、删 cur 具名局部
// （P183/P329）。
bool CInventory::delete_item(INVEN_TYPE invenType, int slot, int count,
                             eItemDelReason reason, bool bLog)
{
    // b28d-Inventory2：623/623 AE（原 625/617），p167 余 cMyTrace C1/C2 同址伪差。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    if (count < 1)
    {
        return 0;
    }
    // b28d-Inventory2：ORIG 0x0850405a 起为 switch(INVEN_TYPE) 二分决策树
    // （cmp $1/je、cmp $1/jg、test/je、cmp $2、cmp $3），探针实测 4.4.7 同形；
    // 叶块物理序 = case 书写序 0,1,2,3（P324）。历史 if-else 链为误译。
    switch (invenType)
    {
    case INVEN_TYPE_INVEN:
        if (m_invenItem[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (bLog)
        {
            if (m_pParent != 0)
            {
                    m_pParent->m_historyLog.ItemDel(invenType, 0, 1, m_invenItem[slot], reason);
            }
            else
            {
                cMyTrace(
                    __PRETTY_FUNCTION__,
                    0x1811, 5)("History ERROR, m_pParent NULL, ItemDel ");
            }
        }
        m_invenItem[slot].reset();
        if (slot < 10)
        {
            SendAvatarEvent(0x33, slot, 0, 0, 0);
        }
        break;

    case INVEN_TYPE_EQUIP:
        if (slot == 0)
        {
            return use_money(count, (eMoneySubReason)9, bLog);
        }
        if (m_pEquipSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (m_pEquipSlot[slot].m_field1 == 1)
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.ItemDel(invenType, 0, 1, m_pEquipSlot[slot], reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x1831, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else if (m_pEquipSlot[slot].m_addInfo2 == count)
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.ItemDel(invenType, 0, m_pEquipSlot[slot].m_addInfo2,
                                                        m_pEquipSlot[slot], reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x184d, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else
        {
            // b28d-Inventory2：ORIG 0x85043b8 `sub; test; jle <log 块>`（log 为跳转
            // 目标、正常流程直落）⇒ 必须写正向 if/else，早返回形会产 `jg <after>`。
            if (m_pEquipSlot[slot].m_addInfo2 - count > 0)
            {
                m_pEquipSlot[slot].m_addInfo2 = m_pEquipSlot[slot].m_addInfo2 - count;
                if (bLog)
                {
                    if (m_pParent != 0)
                    {
                            m_pParent->m_historyLog.ItemDel(invenType, m_pEquipSlot[slot].m_addInfo2, count,
                                                            m_pEquipSlot[slot], reason);
                    }
                    else
                    {
                        cMyTrace(
                            __PRETTY_FUNCTION__,
                            0x1868, 5)("History ERROR, m_pParent NULL, ItemDel ");
                    }
                }
            }
            else
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__,
                    0x186f, "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                    m_pEquipSlot[slot].m_addInfo2, count);
                return 0;
            }
        }
        break;

    case INVEN_TYPE_AVATAR:
    {
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (bLog)
        {
            if (m_pParent != 0)
            {
                    m_pParent->m_historyLog.AvatarItemDel(m_pAvatarSlot[slot].m_addInfo,
                                                          m_pAvatarSlot[slot].m_addInfo2,
                                                          m_avatarItemMgr.GetIPGAgencyNo(
                                                              m_pAvatarSlot[slot].m_addInfo2),
                                                          (eAvatarItemDelReason)reason);
            }
            else
            {
                cMyTrace(
                    __PRETTY_FUNCTION__,
                    0x187e, 5)("History ERROR, m_pParent NULL, ItemDel ");
            }
        }
        m_pAvatarSlot[slot].reset();
        break;
    }

    case INVEN_TYPE_CREATURE:
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (m_pCreatureSlot[slot].isEquipableItemType())
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.CreatureItemDel(
                            invenType, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2, 1,
                            m_pCreatureSlot[slot].m_field1, reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x18a3, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pCreatureSlot[slot].reset();
        }
        else
        {
            if (m_pCreatureSlot[slot].m_addInfo2 == count)
            {
                if (bLog)
                {
                    if (m_pParent != 0)
                    {
                            m_pParent->m_historyLog.CreatureItemDel(
                                invenType, m_pCreatureSlot[slot].m_addInfo, 0,
                                m_pCreatureSlot[slot].m_addInfo2, m_pCreatureSlot[slot].m_field1,
                                reason);
                    }
                    else
                    {
                        cMyTrace(
                            __PRETTY_FUNCTION__,
                            0x18bb, 5)("History ERROR, m_pParent NULL, ItemDel ");
                    }
                }
                m_pCreatureSlot[slot].reset();
            }
            else
            {
                // ORIG 0x850484f `sub; test; jle <log 块>`（同上，正向 if/else）。
                if (m_pCreatureSlot[slot].m_addInfo2 - count > 0)
                {
                    m_pCreatureSlot[slot].m_addInfo2 = m_pCreatureSlot[slot].m_addInfo2 - count;
                    if (bLog)
                    {
                        if (m_pParent != 0)
                        {
                                m_pParent->m_historyLog.CreatureItemDel(
                                    invenType, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2,
                                    count, m_pCreatureSlot[slot].m_field1, reason);
                        }
                        else
                        {
                            cMyTrace(
                                __PRETTY_FUNCTION__,
                                0x18d2, 5)("History ERROR, m_pParent NULL, ItemDel ");
                        }
                    }
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x18da, "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                        ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                        m_pCreatureSlot[slot].m_addInfo2, count);
                    return 0;
                }
            }
        }
        break;
    }
    }
    return 1;
}