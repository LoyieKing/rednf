void CClearReward::_generateSpecificItem(stGenerateRefClearReward_t const& refClear,
                                         stGenerateResult_t& result)
{
    // ORIG generateNumber 结果不落槽（P0 内联右值，P134 嵌套实参形）：
    // mov 0xc(%ebp),%edx; mov 0xc(%edx),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)。
    // ORIG je 直落统一 leave 出口：if(!=-2){主体} 形消除 4.4 jmp/nop 中继
    //（P63 早退共享出口，探针实证）。
    int itemIdx = refClear.m_dungeon->get_clear_item(m_random.generateNumber(4, 100));
    if (itemIdx != -2)
    {
        Inven_Item inv;
        inv.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        result.m_dropItems.push_back(inv);
    }
}