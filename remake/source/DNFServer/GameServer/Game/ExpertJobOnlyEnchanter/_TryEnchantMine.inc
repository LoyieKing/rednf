int CEnchanter::_TryEnchantMine(CUser* user, stItemPos_t* pos)
{
    int ret = 0;
    if ((ret = m_materials[0].SetData(user, (unsigned short)pos->m_itemSpace,
                                      pos->m_slot)) != 0)
        return ret;

    CItem* card = FindMonsterCard(reinterpret_cast<CItem*>(m_field40));
    if (card == 0)
        return 0x11;

    Inven_Item invenItem;
    int slot = -1;
    // [b28d-solo-ExpertJobOnlyEnchanter] P171/P232：ORIG 0x0849d83d 有死写入
    // `movl $0x0,-0xc(%ebp)`（Ghidra local_10 赋 0 后不再读）⇒ 照抄一个死局部，
    // 同时把 4.4 标量槽区补到与 ORIG 同深（ret -0x18 / card -0x14 / slot -0x10）。
    int dead = 0;
    (void)dead;
    // P15：赋值与条件分两句（ORIG 为 `cmpl $0xffffffff,-0x10; jne` 直跳，
    // 赋值即条件形会多 sete;test 物化）。
    slot = user->getCurCharacInvenR()->GetInvenData(card->get_index(),
                                                    invenItem);
    if (slot == -1)
        return 0x11;

    if ((ret = m_materials[1].SetData(user, 0, (unsigned short)slot)) != 0)
        return ret;

    if (!IsRecipeRight())
        return 0x11;

    if (!IsLearnRecipe(
            user, static_cast<CStackableItem const*>(
                      reinterpret_cast<CStackableItem*>(m_field40))))
        return 0x11;

    if ((ret = m_materials[0].CheckType(false, 0)) != 0)
        return ret;

    if ((ret = m_materials[1].CheckType(true, 2)) != 0)
        return ret;

    m_field3c = m_materials[1].m_item;
    if (reinterpret_cast<CItem*>(m_field3c) == 0)
        return 0x11;

    if (!IsEqualMonsterCad())
        return 0x11;

    if (!::EnableWorld(reinterpret_cast<CItem*>(m_materials[0].m_item),
                       reinterpret_cast<CItem*>(m_field40)))
        return 0x11;

    // ORIG 虚链：target 卡对象 vtable+0x20 槽（getUsableItemType(int)），实参取
    // 材料 0 物品 vtable+0xc 槽（GetItemType()）返回值。
    if (!reinterpret_cast<CItem*>(m_field3c)->getUsableItemType(
            reinterpret_cast<CItem*>(m_materials[0].m_item)->GetItemType()))
        return 0x17;

    if ((ret = _DoEnchant(ENUM_TRY_ENCHANT_TYPE_0)) != 0)
        return ret;
    return 0;
