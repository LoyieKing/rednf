int Dispatcher_UseCreatureEvolutionItem::process(CUser* user,
                                                 unsigned char evolveSlot,
                                                 short itemCount, int itemIdx)
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)evolveSlot);
    const Inven_Item* item =
        user->getCurCharacInvenR()->GetInvenRef(invenType, itemCount);
    if (item == 0)
        return 4;

    CItem* itemInfo = G_CDataManager()->find_item(item->m_addInfo);
    if (itemInfo == 0)
    {
        cMyTrace local_58(__PRETTY_FUNCTION__, 0xe621, 0);
        local_58("Dispatcher_UseCreatureEvolutionItem not found item_id(%d), charac_no(%d)\n",
                 item->m_addInfo, user->getCurCharacNo());
        return 0x15;
    }
    if (!itemInfo->is_stackable())
    {
        cMyTrace local_48(__PRETTY_FUNCTION__, 0xe627, 0);
        local_48("Dispatcher_UseCreatureEvolutionItem item_id(%d) is not stackable charac_no(%d)\n",
                 item->m_addInfo, user->getCurCharacNo());
        return 0x17;
    }

    CStackableItem* stackable = static_cast<CStackableItem*>(itemInfo);
    // [b23S3C] ORIG 0x08228bcc：本比较无 creatureIdx 槽（P0——`call; mov 0x360(%edx),%edx;
    // cmp %edx,%eax; setne;test;je` call 结果直比，P197④ 4.4 同出 setne）；链式三 call
    // 直传。残余 = ORIG 三段 cMyTrace 实参 call 在 ctor 前（4.1 跨 ctor hoist，4.4 不可
    // 复现）+ ebx/esi/edi 跨 call 缓存 vs 4.4 槽 ⇒ 布局封顶 N。
    if (user->getCurCharacInvenW()->GetCreatureMgrW()->GetCreatureIndex()
        != stackable->m_int360)
    {
        cMyTrace local_38(__PRETTY_FUNCTION__, 0xe62f, 0);
        local_38("Dispatcher_UseCreatureEvolutionItem cur_creatureid(%d) != item_base_creature_id(%d) charac_no(%d)\n",
                 user->getCurCharacInvenW()->GetCreatureMgrW()->GetCreatureIndex(),
                 stackable->m_int360, user->getCurCharacNo());
        return 7;
    }

    if (user->getCurCharacInvenRefW()->GetCreatureMgrW()->ItemEvolute(itemIdx))
    {
        cHistoryTrace local_26;
        local_26("ItemEvolute Succ evolution_creature_id(%d), charac_no(%d)",
                 itemIdx, user->getCurCharacNo());
        return 0;
    }
    cHistoryTrace local_25;
    local_25("ItemEvolute Fail evolution_creature_id(%d), charac_no(%d)",
             itemIdx, user->getCurCharacNo());
    return 1;
