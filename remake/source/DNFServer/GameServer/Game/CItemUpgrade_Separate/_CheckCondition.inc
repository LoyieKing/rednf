int CItemUpgrade_Separate::_CheckCondition(const CUser* /*user*/, const Inven_Item& item,
                                           const CItem* citem, const Inven_Item& matItem,
                                           const GenuineDamageUpgradeTable& table,
                                           int level) const
{
    const unsigned char growth = static_cast<unsigned char>(
        reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        table.m_materials.find(static_cast<int>(growth));
    if (mit == table.m_materials.end())
    {
        return 1;
    }
    const int materialItemIdx = mit->second.m_materialItemIdx;   // ORIG -0x1c 具名槽
    if (matItem.m_addInfo != materialItemIdx)
    {
        return 0x16;
    }

    // [b28d-solo-CItemUpgrade_Separate] ORIG 0x0811e1e2：haveCount 直用 matItem.m_addInfo2
    // （%edi 跨调用保活），rarity/itemType/sepLv 全部内联实参（%ebx/%esi/%edx，右→左求值）；
    // P282/P48：0x0811e1fa 与 0x0811e24e 两次独立虚调 citem->GetItemType()（vptr+0xc）。
    if (matItem.m_addInfo2 <
        static_cast<unsigned int>(const_cast<GenuineDamageUpgradeTable&>(table)
            .getGenuineUpgradeMaterialCnt(
                growth,
                static_cast<int>(static_cast<unsigned char>(
                    item.m_upgradeSep.GetUpgradeSeparate())),
                static_cast<ENUM_EQUIPMENTTYPE>(citem->GetItemType()),
                static_cast<ENUM_RARITY>(citem->get_rarity()))))
    {
        return 0x16;
    }
    if (citem->GetItemType() != 10)
    {
        return 0x13;
    }
    // ORIG 0x0811e271：movzwl 0xb(%eax),%eax + movzwl %ax,%ebx（双零扩展 ⇒ 32 位比较）
    if (static_cast<int>(item.m_fieldb) !=
        static_cast<int>(
            reinterpret_cast<const CEquipItem*>(citem)->get_endurance()))
    {
        return 7;
    }
    // [b28d-51] P48：ORIG 0x0811e29c 末检再次独立调 GetUpgradeSeparate
    // （不复用 sepLv 局部）——p167 差集曾缺 1 个该调用。
    // ORIG 0x0811e29c：`lea 0x1(%eax),%edx; mov 0x1c(%ebp),%eax; mov 0x4(%eax),%eax;
    // cmp %eax,%edx; setg` ⇒ 先算 sepLv+1（左操作数）再读 m_maxLevel（P243 求值序）。
    if (static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()) + 1 > table.m_maxLevel)
    {
        return 0x5f;
    }
    return 0;
