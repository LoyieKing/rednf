// b21 NEAR：语义与 ORIG 0x08499f84 全等（报告全文已读；CheckEquipable/isIdentified 双查、
//   IsAvatarItem/IsCreatureItem/IsEquipable/PeriodAfterUnsealing、setCopy 双方向均逐条核对）。
//   残余：字符串/__PRETTY_FUNCTION__/数据地址【AE】、虚表调用寄存器分配（mov %edx vs mov %eax）、
//   setne 位置（N-86b）。帧/switch 早返结构一致。
// b28d-65：残余 = N7（间接 call 目标 4.4.7 恒 %eax vs ORIG %edx；P230 补：4.4.6 可修，
//   本波不改路由表）+ 比较物化岛（setne+test）+ 对齐 nop ⇒ 封顶。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
int CExpandEquipslot::Move_Item(CUser* pUser, int type, int slot, int space,
                                int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x17c, "Fail Move_Item : pUser is NULL");
        return 0x11;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x182, "Fail Move_Item : pUser is not valid state");
        return 0x11;
    }
    CInventory* inven = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
        switch (space)
        {
        case 0x12:
        {
            if (!CheckEquipable(
                    inven->m_pEquipSlot[slot],
                    m_equipSlot[expandSlot]))
            {
                return 0x11;
            }
            if (!inven->m_pEquipSlot[slot].m_amp.isIdentified() ||
                !m_equipSlot[expandSlot].m_amp.isIdentified())
            {
                return 0x11;
            }
            Inven_Item localItem;
            localItem.setCopy(inven->m_pEquipSlot[slot]);
            if (localItem.m_addInfo != 0)
            {
                CItem* item =
                    G_CDataManager()->find_item(localItem.m_addInfo);
                if (item == 0 || pUser == 0)
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x1a0, 5)(
                        "CExpandEquipslot::Move_Item Move_Item return false (%d)",
                        localItem.m_addInfo);
                    return 0x11;
                }
                if (item->IsAvatarItem())
                {
                    return 0x11;
                }
                if (item->IsCreatureItem())
                {
                    return 0x11;
                }
                int nEquip = 0;
                nEquip = IsEquipable(pUser, item, expandSlot + 10);
                // b28d-65 [P225]：ORIG 0x0849a1da 段 `cmpl $0,-0x10; setne %al;
                // test %al,%al; je`（4.1 对 int 条件物的物化）；4.4 对 if 恒折
                // `cmpl;je`。while+return 形复现（探针 t23/p2_ee.o 逐条同形，含预清）。
                while (nEquip != 0)
                {
                    return nEquip;
                }
                if (item->PeriodAfterUnsealing() != 0)
                {
                    int nExpire = GlobalData::s_systemTime_.getCurSec() +
                                  item->PeriodAfterUnsealing() * 0x15180;
                    if (nExpire != 0)
                    {
                        localItem.m_fieldb =
                            (unsigned short)((nExpire - 0x44a54a80) /
                                             0x15180);
                    }
                }
            }
            inven->m_pEquipSlot[slot].setCopy(
                m_equipSlot[expandSlot]);
            localItem.m_field0 = 0;
            m_equipSlot[expandSlot].setCopy(localItem);
            return 0;
        }
        default:
            return 0x11;
        }
    default:
        return 0x11;
    }
}