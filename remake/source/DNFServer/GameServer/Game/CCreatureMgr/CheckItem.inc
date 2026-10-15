// ORIG _ZN13user_creature12CCreatureMgr9CheckItemEP5CUserP10Inven_ItemRb
// T 0x0833c240（CheckItem.md）：默认 outFlag=true；仅当物品为堆叠宠物蛋类
// （m_field1==7 ITEM_TYPE_CREATURE_STACK 且 m_addInfo==0x1a）时置 outFlag=
// false 并委托 SetKeepRightItem 清占用（其返回值即函数返回）；否则返 true。
bool CCreatureMgr::CheckItem(CUser* user, Inven_Item* item, bool& b)
{
    b = true;
    if (item->m_itemType == Inven_Item::ITEM_TYPE_CREATURE_STACK &&
        item->m_addInfo == 0x1a)
    {
        b = false;
        return SetKeepRightItem(user, item);
    }
    return true;
}
