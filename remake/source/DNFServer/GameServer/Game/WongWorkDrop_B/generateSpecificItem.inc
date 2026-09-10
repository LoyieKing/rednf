void CMonsterDrop_Hell::generateSpecificItem(stGenerateRefData_t const& ref,
                                             stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（157/156；p167 CLEAR 16/16）。
    // 修法 = ctx 指针局部(-0x2c) + 4 平铺早退 nop 岛 + e.w[] disp 折叠。
    // 残差 = UseLuckPoint 实参寄存器角色互换（ORIG rarity→edx/m_field1→eax，
    // 4.4 反向），N7 封顶（cast/内联实参多形穷举均不翻）。
    // [b28d-38] ORIG 0x08535726 逐条：ctx 指针局部(-0x2c) + 四个平铺早退
    // （每个早退生成 nop 中继岛，末尾 return 直落函数尾）。
    stGenerateRefCtx_t const& ctx =
        *reinterpret_cast<stGenerateRefCtx_t const*>(&ref.m_field34);

    int idx =
        _getGenItemProbIdx(static_cast<int>(static_cast<unsigned char>(ctx.m_field35)));
    if (idx < 0)
        return;

    stGenItemProb_t const& e = m_probsSub[idx];      // ORIG op[] on +0x24
    // 阈值 = e.w[(byte)ctx.m_rateSlot] * ref.m_dropRate / 100（ORIG disp=8 折叠）
    int threshold =
        e.w[static_cast<unsigned char>(ctx.m_rateSlot)] * ref.m_dropRate / 100;
    if (m_random.generateNumber(0, 1000) > threshold)
        return;

    int luckRoll = m_random.generateNumber(1, 1000000);
    int basisRow = static_cast<int>(static_cast<signed char>(ref.m_field42_byte));
    // GetItemRarity(&m_rarityBasis[basisRow], luckRoll, dropType=2)
    int rarity =
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)->GetItemRarity(
            &m_rarityBasis[basisRow][0], luckRoll, 2);

    int itemIdx = m_table.chooseItem(eItemDropType_2,
                                     static_cast<ENUM_RARITY>(rarity),
                                     static_cast<unsigned char>(ctx.m_field35));
    if (itemIdx == -1)
        return;

    // [b28d-38] P167：ORIG 无 getItemList() call（头内 inline 在 4.4-O0 发
    // out-of-line call，ORIG 是 `mov 0xc(%eax),%eax` 直读 m_itemList）⇒ 具名成员直访。
    CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (item == 0)
        return;

    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    // 实参内联（右→左求值：size→get_grade→get_rarity→m_field1，ORIG esi/ebx 缓存）
    reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
        ->UseLuckPoint(inv.m_field1, item->get_rarity(), item->get_grade(),
                       static_cast<int>(result.m_dropItems.size()));
    result.m_dropItems.push_back(inv);
