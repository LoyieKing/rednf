void Inter_LoadInven::_CheckUsableExpertJobEquipment(CUser* user)
{
    if (user->getCurCharacR() == 0)
        return;
    CInventory* inven = user->getCurCharacInvenW();
    for (int slot = 0xa; slot <= 0x15; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(0, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0)
            continue;
        if (ci->is_stackable())
            continue;
        if (!ci->isExpertJobUsable(
                (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp())))
        {   // 专家职业不匹配 → 清槽并回收进背包
            inven->ResetSlot(0, slot);
            if (inven->insertItemIntoInventory(item, (eItemAddReason)0xe, false, false)
                & 0x80000000)
            {   // ORIG shr $0x1f 位测：插入失败 → 邮件兜底
                WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, item, 0u,
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    true, reinterpret_cast<const char*>(0), 0);
            }
        }
    }
