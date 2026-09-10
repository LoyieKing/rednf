int CInventory::move_item(INVEN_TYPE invenType, int slot, INVEN_TYPE dstType, int dstSlot)
{
    // b28d-solo-CInventory：1282/1290，P167 CLEAR（80/80）；N82/N80/N6 封顶。
    // b28d-37 [N82 封顶，ORIG 0x08500688]：P167 调用点差集 CLEAR（80/80）；帧/槽复用残差（1282/1290），语义闭合。
    // [b28d-tu-CInventory] 本轮复核（struct 1463 为 difflib 巨型件 autojunk
    //   口径放大，N126 已录）：主体 = ① cMyTrace 变参 4 实参 ORIG 于 ctor 前
    //   一次物化 edi/esi/ebx、4.4 于装载点逐条读 ⇒ N80；② `if (dstType==1)
    //   {EQUIP↔EQUIP 大块}` ORIG 4.1 大块后置（je <远块>）热路前置，4.4 恒
    //   then-inline ⇒ N104（insert_item 三形实测同族，源码级不可翻）；
    //   ③ sete/setne 物化方向 ×2（N6/P248 三档已试）。无具名中间局部可删
    //   （P329 不适用）、平移非恒定常数（4.1 专属槽族）⇒ 维持封顶。
    if (!(CheckValidSlot((int)invenType, slot) == 1 &&
          CheckValidSlot((int)dstType, dstSlot) == 1))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x112c, 5);
        trace("CInventory::move_item move_item return false 0, %d,%d,%d,%d",
              (int)invenType, slot, (int)dstType, dstSlot);
        return 0x11;
    }
    if ((unsigned)invenType < 3 && dstType == INVEN_TYPE_CREATURE)
    {
        std::swap<int>(slot, dstSlot);
        std::swap<INVEN_TYPE>(invenType, dstType);
    }
    if ((int)invenType == 1)
    {
        if ((int)dstType == 1)
        {
            // ---- EQUIP ↔ EQUIP ----
            int srcType = GetSlotItemType(slot);
            int dstItemType = GetSlotItemType(dstSlot);
            if (srcType == Inven_Item::ITEM_TYPE_INVALID ||
                dstItemType == Inven_Item::ITEM_TYPE_INVALID)
            {
                return 0x11;
            }
            if (srcType != 0 && dstItemType != 0 && srcType != dstItemType)
            {
                return 0x11;
            }
            if (srcType != 0 && m_pEquipSlot[dstSlot].m_addInfo != 0 &&
                (int)m_pEquipSlot[dstSlot].m_field1 != srcType)
            {
                return 0x11;
            }
            if (dstItemType != 0 && m_pEquipSlot[slot].m_addInfo != 0 &&
                (int)m_pEquipSlot[slot].m_field1 != dstItemType)
            {
                return 0x11;
            }
            Inven_Item tmp;
            tmp.setCopy(m_pEquipSlot[slot]);
            m_pEquipSlot[slot].setCopy(m_pEquipSlot[dstSlot]);
            m_pEquipSlot[dstSlot].setCopy(tmp);
            return 0;
        }
        if ((int)dstType > 1)
        {
            if ((int)dstType == 2)
            {
                return 0x11;
            }
            if ((int)dstType != 3)
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- EQUIP(src) → INVEN(dst)：脱下并换回原背包格 ----
        if (!CheckEquipable(m_pEquipSlot[slot], m_invenItem[dstSlot]))
        {
            return 0x11;
        }
        if (!m_pEquipSlot[slot].m_amp.isIdentified() ||
            !m_invenItem[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (GetSlotItemType(slot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        Inven_Item tmp;
        tmp.setCopy(m_pEquipSlot[slot]);
        if (tmp.m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(tmp.m_addInfo);
            if (pItem == 0 || m_pParent == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1276, 5);
                trace("CInventory::move_item move_item return false 4,%d", tmp.m_addInfo);
                return 0x11;
            }
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, dstSlot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() && m_pEquipSlot[slot].m_random.m_random.empty())
            {
                return 0x11;
            }
            if (pItem->PeriodAfterUnsealing() != 0)
            {
                int nExpire = GlobalData::s_systemTime_.getCurSec() +
                              pItem->PeriodAfterUnsealing() * 0x15180;
                if (nExpire != 0)
                {
                    tmp.m_fieldb =
                        (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
                }
            }
        }
        m_pEquipSlot[slot].setCopy(m_invenItem[dstSlot]);
        tmp.m_field0 = 0;
        m_invenItem[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)invenType > 1)
    {
        if ((int)invenType == 2)
        {
            if ((int)dstType == 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1306, 5);
                trace("CInventory::move_item move_item return false 8");
                return 0x11;
            }
            if ((int)dstType == 2)
            {
                // ---- AVATAR ↔ AVATAR ----
                int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
                int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
                Inven_Item tmp;
                tmp.setCopy(m_pAvatarSlot[slot]);
                m_pAvatarSlot[slot].setCopy(m_pAvatarSlot[dstSlot]);
                m_pAvatarSlot[dstSlot].setCopy(tmp);
                SendAvatarEvent(0x32, slot + 10, dstSlot + 10, srcAddInfo2, dstAddInfo2);
                return 0;
            }
            if ((int)dstType != 0)
            {
                return 0;
            }
            // ---- AVATAR(src) → INVEN(dst) ----
            Inven_Item tmp;
            tmp.setCopy(m_pAvatarSlot[slot]);
            if (!CheckEquipable(m_pAvatarSlot[slot], m_invenItem[dstSlot]))
            {
                return 0x11;
            }
            if (m_invenItem[dstSlot].m_addInfo != 0)
            {
                CItem* pItem = G_CDataManager()->find_item(m_invenItem[dstSlot].m_addInfo);
                if (m_pParent != 0 && pItem != 0)
                {
                    if (!pItem->IsAvatarItem())
                    {
                        return 0x11;
                    }
                    int equipable = IsEquipable(m_pParent, pItem, dstSlot);
                    if (equipable != 0)
                    {
                        return equipable;
                    }
                }
            }
            int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
            int dstAddInfo2 = m_invenItem[dstSlot].m_addInfo2;
            MoveClearAvatar(slot + 10, srcAddInfo2, m_pAvatarSlot[slot].m_addInfo,
                            dstSlot, dstAddInfo2, m_invenItem[dstSlot].m_addInfo);
            m_pAvatarSlot[slot].setCopy(m_invenItem[dstSlot]);
            m_invenItem[dstSlot].setCopy(tmp);
            SendAvatarEvent(0x32, slot + 10, dstSlot, srcAddInfo2, dstAddInfo2);
            return 0;
        }
        if ((int)invenType != 3)
        {
            return 0;
        }
        if ((int)dstType == 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x131c, 5);
            trace("CInventory::move_item move_item return false 9");
            return 0x11;
        }
        if ((int)dstType == 3)
        {
            // ---- CREATURE ↔ CREATURE（首参为目的侧，与 ORIG 压栈序一致）----
            // ORIG 0x08501752 `mov 0x8(%ebp),%eax; add $0x6e8,%eax` = 直接取
            // &m_creatureMgr 作接收者，不经 GetCreatureMgrW（P167 EXTRA 清零）。
            if (m_creatureMgr.SwapCreatureItem(
                    &m_pCreatureSlot[dstSlot], dstSlot, (int)dstType,
                    &m_pCreatureSlot[slot], slot, (int)invenType))
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- CREATURE(src) → INVEN(dst) ----
        if (m_creatureMgr.SwapCreatureItem(
                &m_invenItem[dstSlot], dstSlot, (int)dstType,
                &m_pCreatureSlot[slot], slot, (int)invenType))
        {
            return 0;
        }
        return 0x11;
    }
    if ((int)invenType != 0)
    {
        return 0;
    }
    if ((int)dstType == 1)
    {
        // ---- INVEN(src) → EQUIP(dst)：穿戴并与原装备交换 ----
        if (!CheckEquipable(m_invenItem[slot], m_pEquipSlot[dstSlot]))
        {
            return 0x11;
        }
        if (!isEquipmentPlaceableAtInventory(dstSlot))
        {
            return 0x11;
        }
        CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() &&
                m_pEquipSlot[dstSlot].m_random.m_random.empty())
            {
                return 0x11;
            }
        }
        if (GetSlotItemType(dstSlot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        if (!m_invenItem[slot].m_amp.isIdentified() ||
            !m_pEquipSlot[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (m_pEquipSlot[dstSlot].m_field0 != 0 &&
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                m_pParent, (RestrictCategory::Enum)1, 0xe))
        {
            return 0xd1;
        }
        Inven_Item tmp;
        tmp.setCopy(m_invenItem[slot]);
        m_invenItem[slot].setCopy(m_pEquipSlot[dstSlot]);
        if (m_pParent != 0 && m_invenItem[slot].m_field0 != 0)
        {
            m_pParent->m_historyLog.seal_Item(
                m_invenItem[slot].m_addInfo,
                (short)m_invenItem[slot].GetReSealCount(),
                eSealItemReason_1);
        }
        m_invenItem[slot].m_field0 = 0;
        if (pItem != 0 && pItem->PeriodAfterUnsealing() != 0)
        {
            int nExpire = GlobalData::s_systemTime_.getCurSec() +
                          pItem->PeriodAfterUnsealing() * 0x15180;
            if (nExpire != 0)
            {
                m_invenItem[slot].m_fieldb =
                    (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
            }
        }
        m_pEquipSlot[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)dstType < 2)
    {
        if ((int)dstType != 0)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x122a, 5);
        trace("CInventory::move_item move_item return false 3");
        return 0x11;
    }
    if ((int)dstType != 2)
    {
        if ((int)dstType != 3)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1224, 5);
        trace("CInventory::move_item move_item return false 10");
        return 0x11;
    }
    // ---- INVEN(src) → AVATAR(dst) ----
    if (!CheckEquipable(m_invenItem[slot], m_pAvatarSlot[dstSlot]))
    {
        return 0x11;
    }
    if (m_pAvatarSlot[dstSlot].m_addInfo != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(m_pAvatarSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (!pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
        }
    }
    int srcAddInfo2 = m_invenItem[slot].m_addInfo2;
    int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
    MoveClearAvatar(dstSlot + 10, dstAddInfo2, m_pAvatarSlot[dstSlot].m_addInfo,
                    slot, srcAddInfo2, m_invenItem[slot].m_addInfo);
    Inven_Item tmp;
    tmp.setCopy(m_invenItem[slot]);
    m_invenItem[slot].setCopy(m_pAvatarSlot[dstSlot]);
    m_pAvatarSlot[dstSlot].setCopy(tmp);
    SendAvatarEvent(0x32, slot, dstSlot + 10, srcAddInfo2, dstAddInfo2);
    return 0;
