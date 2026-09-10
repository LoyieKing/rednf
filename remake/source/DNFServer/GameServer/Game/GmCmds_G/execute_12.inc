void CTestTradeRestrict::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    // ---- 循环 1：m_pEquipSlot 数组（0x39 槽）----
    for (int i = 0; i < 0x39; ++i)
    {
        // ORIG 每用点 fresh 取 getter/基址（P48），右值表达式寻址（P0 无具名）
        if (user->getCurCharacInvenR()->m_pEquipSlot[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_pEquipSlot[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_pEquipSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 2：inventory 对象内嵌数组（0x1a 槽，m_invenItem）----
    for (int i = 0; i < 0x1a; ++i)
    {
        if (user->getCurCharacInvenR()->m_invenItem[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_invenItem[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_invenItem[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 3：m_pAvatarSlot 数组（0x69 槽，仅判 itemIndex）----
    for (int i = 0; i < 0x69; ++i)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo != 0)
            user->getCurCharacInvenW()->m_pAvatarSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 4：角色仓库 CCargo（0x98 槽）----
    Inven_Item* cargoStart =
        const_cast<Inven_Item*>(user->getCurCharacCargoW()->get_cargo_start());
    for (int i = 0; i < 0x98; ++i)
    {
        if (cargoStart[i].m_addInfo != 0 &&
            cargoStart[i].m_field1 == 1)            // ITEM_TYPE_EQUIP
        {
            cargoStart[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
    }

    // ---- 账号货物（GetSlotRef(0)，0x38 槽；引用为空则跳过）----
    Inven_Item* accSlot = user->GetAccountCargo()->GetSlotRef(0);
    int i = 0;
    while (i < 0x38 && accSlot != 0)
    {
        if (accSlot[i].m_addInfo != 0 &&
            accSlot[i].m_field1 == 1)               // ITEM_TYPE_EQUIP
        {
            accSlot[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
        ++i;
    }

    DB_TestTradeRestrict::makeRequest((unsigned int)user->getCurCharacNo());
