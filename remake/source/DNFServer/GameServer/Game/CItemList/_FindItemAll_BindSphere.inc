int CCompound_ExtreamDun_Item::_FindItemAll_BindSphere(             // ORIG T 0x832d438
    const CEquipItem** refs, char select, ENUM_OUTPUTITEMS_RARITY outRarity,
    std::pair<unsigned int, unsigned int> range) const
{
    // ORIG 0x832d438：比较对象是 char select（spill -0x1c，cmpb），
    // 结果统一落 slot -0xc（int result = 0 初存 + 两支 call 后写回）。
    int result = 0;
    if (select == 2)
    {
        result = _FindRandomItem(outRarity, range);
    }
    else if (select == 0 || select == 1)
    {
        result = _FindSelectItem(refs[select]->getItemGroupName(), outRarity, range);
    }
    return result;
