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

        // ORIG 084c4e44 mov -0x14(%ebp),%eax; mov %eax,-0x10(%ebp)：static_cast<CEquipItem*>
        //   的副本在 check_low_level 之前落地（GetUsableMaxLevel 用该副本 -0x10，GetAttachType
        //   用 ci -0x14）。
        CEquipItem* cequip = static_cast<CEquipItem*>(ci);
        // P183：ORIG 无 needMove 具名槽（084c4e9c mov $0x1,%eax / 084c4ea3 mov $0x0,%eax）
        //   ⇒ 原码是单个短路 if；ORIG 3 次 get_charac_level（O0 不 CSE）：levelGap +
        //   check_low_level + GetUsableMaxLevel。
        // 语义轴：ORIG 084c4e69 `xor $0x1,%eax; test %al,%al; jne 084c4e9c` ⇒ 原码是
        //   `!check_low_level(...)`（CItem::check_low_level = m_usableLevel <= level，即等级
        //   不够才移走）；旧稿写成正向条件，判定反了。
        if (!ci->check_low_level(user->get_charac_level() + overLevel)
            || (ci->GetAttachType() == 5
                && cequip->GetUsableMaxLevel() < user->get_charac_level()))
        {
            if (ci->IsAvatarItem())                     // vtable+0x10 槽
            {   // 化身位 → 移入 avatar 仓空槽
                int emptySlot = -1;                     // 084c4e8e movl $0xffffffff,-0xc(%ebp)
                emptySlot =
                    inven->get_empty_slot((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8);
                if (emptySlot != -1)
                    inven->move_item((INVEN_TYPE)0, slot, (INVEN_TYPE)2, emptySlot);
            }
            else
            {
                inven->ResetSlot(0, slot);              // 仓库重置 → 回背包并邮寄兜底
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
    }
    user->resetCurCharacOverEquipFlag();
