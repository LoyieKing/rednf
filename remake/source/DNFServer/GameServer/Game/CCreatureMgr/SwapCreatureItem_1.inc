// ORIG _ZN13user_creature12CCreatureMgr16SwapCreatureItemEP10Inven_ItemiiS2_ii
// T 0x083399fa：按物品类型分流到 SwapCreature/SwapArtifact，最后交换两物品内容。
bool CCreatureMgr::SwapCreatureItem(Inven_Item* item1, int slot1, int type1,
                                    Inven_Item* item2, int slot2, int type2)
{
    if (item1->m_itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
        item2->m_itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
    {
        if (!SwapCreature(item1, slot1, type1, item2, slot2, type2))
            return false;
    }
    else if (item1->m_itemType == Inven_Item::ITEM_TYPE_ARTIFACT ||
             item2->m_itemType == Inven_Item::ITEM_TYPE_ARTIFACT)
    {
        if (!SwapArtifact(item1, slot1, type1, item2, slot2, type2))
            return false;
    }

    std::swap(*item1, *item2);           // ORIG _ZSt4swapI10Inven_ItemEvRT_S2_
    return true;
}
