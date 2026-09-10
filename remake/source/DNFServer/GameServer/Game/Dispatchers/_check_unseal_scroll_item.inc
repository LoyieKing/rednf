int Dispatcher_UnsealRandomOption::_check_unseal_scroll_item(
    CUser* user, const Inven_Item* item, unsigned short slot)
{
    if (slot != 0xffff)
    {
    CInventory* inven = (CInventory*)(const CInventory*)user->getCurCharacInvenR();
        Inven_Item scroll = inven->GetInvenSlot(1, (int)slot);
        CItem* proto =
            G_CDataManager()->find_item(*(const int*)&scroll.m_addInfo);
        if (proto == 0)
        {
            return 0x15;
        }
        if (!proto->is_stackable())
        {
            return 0x11;
        }
        if (reinterpret_cast<const CStackableItem*>(proto)->m_int328 == -1)  // +0x328
        {
            return 1;
        }
        int script =
            (int)(size_t)G_CDataManager()->GetExpertJobScript(2);   // ORIG 参数常量 2
        int possibleLevel =
            ((STExpertJobScript*)script)->GetUnsealScrollPossibleLevel(1 /*占位*/ );
        // GetUnsealScrollPossibleLevel 无参版本按报告签名还原见下（重载澄清）：
        int limit = possibleLevel;
        if (limit == -1)
        {
            return 1;
        }
        CItem* base = G_CDataManager()->find_item(item->m_addInfo);
        if (base != 0 && limit < base->getUsableLevel())
        {
            return 0x11;
        }
    }
    return 0;
