// 0x0867b6d4
// ORIG：添加物品。角色空/交易中/物品不存在/count<0 → -1；
// 构造 Inven_Item（make_item），stackable 则 set_add_info(count)；
// *space 默认 0（生物物品 → 7）；slot∈(0,0x20) 且为装备 → SetUpgrade(slot)；
// insertItemIntoInventory(reason, true, true)，失败 -1；
// 生物物品再 InsertCreatureItem。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b6d4 | _ZN5CUser7AddItemEii14eItemAddReasonR14ENUM_ITEMSPACEi
int CUser::AddItem(int itemIdx, int count, eItemAddReason reason,
                   ENUM_ITEMSPACE& space, int slot)
{
    if (getCurCharacR() == 0)
        return -1;
    if (CheckInTrade() != 0)
        return -1;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    if (count < 0)
        return -1;
    Inven_Item inven;
    // 2026-09-11（H3CUser，语义轴）：ORIG 0x0867b6d4 在 `call Inven_Item::C1Ev`
    //   之后、`make_item` 虚调用**之前**写 `mov 0xc(%ebp),%eax; mov %eax,-0x5b(%ebp)`
    //   = inven+0x02 的 m_addInfo（unsigned int item idx，Inven_Item.h:269）。
    //   我方旧形缺此写入（make_item 前 inven 的 item idx 仍为 0）⇒ 补。
    inven.m_addInfo = (unsigned int)itemIdx;
    item->make_item(inven);
    if (item->is_stackable() != 0)
        inven.set_add_info(count);
    space = (ENUM_ITEMSPACE)0;
    if (inven.IsCreatureItemType() != 0)
        space = (ENUM_ITEMSPACE)7;
    if (slot > 0 && slot < 0x20 && inven.m_itemType == (unsigned char)1)
        inven.SetUpgrade((unsigned char)slot);
    // 2026-09-11 CuA A组 P183/P0：ORIG 0x0867b7e9 处 `call getCurCharacInvenW` 结果
    //   **无具名槽**（返回值直接 `mov %eax,(%esp)` 作 this，我方旧具名 invenW 占
    //   -0x18 槽 ⇒ 帧 0xb4 vs ORIG 0xac、全帧槽号 +4）。
    int ret = getCurCharacInvenW()->insertItemIntoInventory(inven, reason, true, true);
    if (ret < 0)
        return -1;
    if (inven.IsCreatureItemType() != 0)
    {
        // 2026-09-11 CuA A组 P329/P0：ORIG 0x0867b8a3/0x0867b8b0 的 getExpirationDate
        //   （→%esi）/getUsablePeriod（→%ebx）结果**不落具名槽**（跨 GetCreatureMgrW call
        //   由 callee-saved 保活）⇒ 删两个具名局部、内联回实参位（右→左求值序一致）。
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &inven, ret, (int)reason, item->getUsablePeriod(),
            item->getExpirationDate());
    }
    return ret;
}