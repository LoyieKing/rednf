int CraneMinigameManager::startUseMaterial(CUser* user, int slot)
{
    // ORIG 0x080ebe0d：getCurCharacInvenR() 结果免槽直传 GetInvenSlot。
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
    if (item.m_addInfo != m_material)                       // ORIG 0x080ebe3f
        return 0x11;
    // ORIG 0x080ebe4d：needCnt 免槽（movzwl+cwtl 直读），比较用寄存器。
    if (item.m_addInfo2 < (int)(short)m_field14)            // ORIG jge 0x080ebe5a
        return 0x16;
    // ORIG 0x080ebe63：needCnt 第二次重读（movzwl+movswl %ax,%ebx），receiver 免槽。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slot, (int)(short)m_field14,
            (eItemDelReason)0x34, true))
        return 0x16;
    return 0;
