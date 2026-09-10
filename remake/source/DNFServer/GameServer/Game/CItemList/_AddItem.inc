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
