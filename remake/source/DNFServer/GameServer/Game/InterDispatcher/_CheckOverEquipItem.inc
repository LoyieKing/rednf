void Inter_LoadInven::_CheckOverEquipItem(CUser* user)
{
    if (!user->needCurCharacCheckOverEquip())
        return;
    if (user->getCurCharacR() == 0)
        return;

    CInventory* inven = user->getCurCharacInvenW();
    for (int slot = 0; slot <= 0x15; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(0, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || ci->is_stackable())
            continue;

        int overLevel = user->GetPremiumInfo()->GetOverEquipableLevel(
            (ENUM_EQUIPMENTTYPE)slot);   // ORIG 传槽号（0..0x15）
        const int maxEquip = user->GetCurCharacMaxEquipLevel();
        const int levelGap = maxEquip - user->get_charac_level();
        overLevel = std::max(overLevel, levelGap);       // ORIG +0x0f5 call std::max<int>(int&,int&)

        // ORIG 3 次 get_charac_level（O0 不 CSE）：levelGap + check_low_level + GetUsableMaxLevel
        bool needMove = false;                          // bVar1
        if (ci->check_low_level(user->get_charac_level() + overLevel))
            needMove = true;
        else if (ci->GetAttachType() == 5)
        {
            CEquipItem* equip = static_cast<CEquipItem*>(ci);
            if (equip->GetUsableMaxLevel() < user->get_charac_level())
                needMove = true;
        }
        if (!needMove)
            continue;

        if (ci->IsAvatarItem())                         // vtable+0x10 槽
        {   // 化身位 → 移入 avatar 仓空槽
            const int emptySlot =
                inven->get_empty_slot((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8);
            if (emptySlot != -1)
                inven->move_item((INVEN_TYPE)0, slot, (INVEN_TYPE)2, emptySlot);
        }
        else
        {
            inven->ResetSlot(0, slot);                  // 仓库重置 → 回背包并邮寄兜底
            const bool insertFailed =
                (inven->insertItemIntoInventory(item, (eItemAddReason)0xe, false, false)
                 & 0x80000000) == 0;
            if (!insertFailed)
            {
                WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, item, 0,
                    user->getCurCharacNo(), true, 0, 0);
            }
        }
    }
    user->resetCurCharacOverEquipFlag();
