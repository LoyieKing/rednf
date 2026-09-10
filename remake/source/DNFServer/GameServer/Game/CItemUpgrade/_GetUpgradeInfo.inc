bool CItemUpgrade::_GetUpgradeInfo(const Inven_Item* item,
                                   const CItem* itemScript,
                                   const upgrade_table_t& table,
                                   upgrade_info_t* out, int level)
{
    if (item->GetUpgrade() >= 0x1f)
        return 0;
    // ORIG 求值序（实参 R2L）：getGrowthGrade 命名局部先求值（-0x1c），
    // return 内联实参 R2L：GetItemType → GetUpgrade（第二次调用，P48 无 CSE）
    // → get_rarity；临时值跨调用存 callee-saved（P52）。
    int growthGrade = ((CEquipItem*)itemScript)->getGrowthGrade(level);
    return table.get_upgrade_info(
        out,
        static_cast<unsigned char>(itemScript->get_rarity()),
        static_cast<unsigned char>(growthGrade),
        item->GetUpgrade(),
        (ENUM_EQUIPMENTTYPE)itemScript->GetItemType());
