// 0x0868dff8
// ORIG：光环 avatar 装备检查。无角色 → 1；槽 9 无物品/不可装备 → 0x11；
// 物品为光环 avatar 选项 → 回写选项值并返回 0，否则 0x17。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0868dff8 | _ZN5CUser20is_equip_aura_avatarEcRi
int CUser::is_equip_aura_avatar(char slot, int& out)
{
    if (getCurCharacR() == 0)
        return 1;
    // b28d-solo-CUser p167 修正：ORIG 0x0868dff8 调 getCurCharacInvenRefW（非 InvenW）。
    Inven_Item* item = getCurCharacInvenRefW()->GetInvenRef(0, 9);
    if (item == 0)
        return 0x11;
    if (item->isEmpty() != 0)
        return 0x11;
    if (item->isEquipableItemType() != (char)1)
        return 0x11;
    // ORIG 0x868e088-0x868e09c：itemIdx 载 %eax 后 `mov %eax,%ebx` 跨 G_CDataManager
    // call 缓存（push %ebx/帧 0x34），find_item 结果落 -0xc（equip 槽），item 槽 -0x10。
    // P0/N96 去绑定：字段作实参直传（P200 拓宽并入 callee-saved，跨 receiver
    // call 保活），残差记 N18a。
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item->m_addInfo);
    if (equip == 0)
        return 0x11;
    if (equip->is_aura_avatar_option(slot) != (char)1)
        return 0x17;
    out = equip->get_aura_avatar_option_value();
    return 0;
}