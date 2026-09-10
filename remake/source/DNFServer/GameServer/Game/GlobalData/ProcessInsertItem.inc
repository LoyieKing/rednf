int ProcessInsertItem(CUser& user,
                      const InstanceRentalSystem::RentInElem& inElem,
                      const EventRewardItemInfo& reward,
                      InstanceRentalSystem::RentOutput& out, int reason)
{
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    item.SetUpgrade((unsigned char)reward.m_short8);  // 插入路径：先升级后 rentId
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 0;                                   // 背包租借标记
    ri.m_inEquip = 0;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x082717cb：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
