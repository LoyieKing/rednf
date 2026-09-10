int CInventory::AddAvatarItem(int itemIdx, int avatarUid, bool b, char c, char d,
                              const char* agency, eAvatarItemAddReason reason, bool e,
                              unsigned short f)
{
    // b28d-solo-CInventory：208/207。真修：getAvatarSocket 第 2 实参 c→d
    // （ORIG 0x08509c2d `movsbl -0x64(%ebp),%edx`，-0x64 是第 5 参 d 的溢出槽，
    // 序言 0x08509bb3 `mov 0x1c(%ebp),%ecx; mov %cl,-0x64`；c 在 -0x60）。
    // 残余 1 = 间接 call 寄存器角色互换（N7）+ 槽残差。
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0 || !pItem->IsAvatarItem())
    {
        return -1;
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    // [b28d-solo] ORIG 0x08509c2d `movsbl -0x64(%ebp),%edx`：-0x64 是第 5 参 d 的
    // 溢出槽（序言 0x08509bb3 `mov 0x1c(%ebp),%ecx; mov %cl,-0x64`），非 c(-0x60)。
    ((CEquipItem*)pItem)->getAvatarSocket((int)d, emblem);
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            int tick = OS_API::GetDateTimeTick();
            m_pAvatarSlot[i].m_addInfo = itemIdx;
            m_pAvatarSlot[i].m_addInfo2 = (int)m_avatarItemMgr.RegistNotAssignedAvatarItem(
                avatarUid, tick, agency, e, emblem);
            m_pAvatarSlot[i].m_field1 = 8;
            m_pAvatarSlot[i].m_fieldb = (short)c;
            m_pAvatarSlot[i].m_field0 = 0;
            // ORIG AddAvatarItem.md：0x8509d30 movw $0,0xd / 0x8509d60 movw %ax,0xd /
            // 0x8509d76 movw $0,0xf —— +0x0d/+0x0f 各 2 字节，同 union 低/高半字。
            m_pAvatarSlot[i].m_fieldSplit.m_fielddLo = 0;
            CItem* pHidden = pItem;
            if (pHidden->IsHiddenOption())
            {
                m_pAvatarSlot[i].m_fieldSplit.m_fielddLo = f;
            }
            m_pAvatarSlot[i].m_fieldSplit.m_avatarEndur = 0;
            m_pAvatarSlot[i].ResetItemAttr();
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            _SaveAvatarItem(i, m_avatarItemMgr.GetExpireDate(m_pAvatarSlot[i].m_addInfo2), b, c,
                            emblem, agency, m_pAvatarSlot[i].m_fieldd, expansion);
            m_pParent->m_historyLog.AvatarItemAdd(
                itemIdx, 0xffffffff, agency, reason);
            return i;
        }
    }
    return -1;
