// CheckValidItem(int cargoSlot, int count) @0x85c9cc2：
// cargo 槽可操作且非空；装备放行，否则要求剩余数量 - count >= 0。
bool CPrivateStore::CheckValidItem(int cargoSlot, int count)
{
    const CCargo* cargo = m_pSeller->getCurCharacCargoR();
    if (!cargo->IsOperatorable(cargoSlot))
        return false;

    Inven_Item item = cargo->get_cargo_slot(cargoSlot);
    if ((int)item.m_addInfo == 0)
        return false;

    // ORIG 0x085c9cc2 尾部（报告 CheckValidItem.md L…）：
    // `if (!isEquipableItemType())`（bool xor$1 形）内嵌 `< 0` 判（jns 跳过 mov$0 岛），
    // 两路汇合 mov$1 —— 与「两 if return true」布局异，探针+diff 实证。
    if (!item.isEquipableItemType())
    {
        if ((int)item.m_addInfo2 - count < 0)
            return false;
    }
    return true;
}