// [b28d-20] _AddItem（ORIG 0x832dd1c，78/79，nd 35）：ORIG entry@-0x1c、
//   entry2@-0x14（entry 先构造占深槽）；4.4 先声明占浅槽（entry@-0x14），
//   交换声明序会倒转两对象的构造/实参求值序（N93 槽方向律）⇒ 封顶。
//   语义与 ORIG 一致（P234 static_cast<T&&> 已用，无 std::move 外链）。
// [b28d-solo-CItemList] DIFF（78/79，ndiff 13↓）| entry/entry2 改匿名临时实参
//   ⇒ 复现 ORIG 0x0832dd70/-0x1c、0x0832ddc1/-0x14 槽向（具名连排落
//   -0x14/-0x1c 反向，N93）。残 = m_bindItems[group][cat] 二维索引乘法链
//   寄存器角色（ORIG 0x0832dde2 `lea 0x0(,%eax,4),%ecx` 用 ecx；4.4 用
//   eax→edx 多 1 mov）⇒ N86b「同一算术多 codegen」；T5：类型已定（cat/索引
//   均 32 位），禁再改类型（S1 已用一次，unsigned cat 实测零变化）。
// [b28d-tu-CItemList 2026-09-09] 复核：78/79（gap +1）——同 after_addItem 的
//   m_bindItems[group][cat] 索引链 N86b 残差；p167 与 ORIG 调用多重集一致。
bool CCompound_ExtreamDun_Item::_AddItem(CEquipItem& item)          // ORIG 0x832dd1c
{
    // cat=_convertOutputItem(GetUsableEquipmentType)；cat==4 → 不可入表；
    // 否则 {usableLevel,index} 同记结果表[cat] 与合成行表[group][cat]。
    const int cat = _convertOutputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)item.GetUsableEquipmentType());
    if (cat == 4)
    {
        return false;
    }
    // ORIG 槽：entry@-0x1c、entry2@-0x14（连排声明复现槽序）。
    m_data.m_resultItems[cat].push_back(
        OUTPUT_ITEM_STR(item.getUsableLevel(), item.get_index()));
    m_data.m_bindItems[item.getItemGroupName()][cat].push_back(
        OUTPUT_ITEM_STR(item.getUsableLevel(), item.get_index()));
    return true;
}