int CUser::CalcMercenaryAvatarBonus() const
{
    int best = 10;

    if (getCurCharacR() == 0)
        return 0;

    const CInventory* inven = getCurCharacInvenR();
    if (inven == 0)
        return 0;

    for (int i = 0; i < 8; ++i)
    {
        const Inven_Item* ref = inven->GetInvenRef(INVEN_TYPE_INVEN, i);
        if (ref == 0)
            return 0;

        // ref->m_addInfo（+0x2）为 item idx；ORIG 0x0868eaeb 在 find_item 返回 0 时
        // 直接给 equip 赋 0（`test %eax,%eax; jne L; mov $0,%eax; jmp after; L:
        // dynamic_cast`）⇒ 不设 `CItem* item` 具名局部（会多占一槽使全帧深 4）。
        // dynamic_cast<CEquipItem*>(0) 返回 0，语义与 ORIG 一致。
        CEquipItem* equip = dynamic_cast<CEquipItem*>(
            G_CDataManager()->find_item(ref->m_addInfo));
        // P222 正向包裹：ORIG 0x0868eb32 `cmpl $0,-0xc; je <+0x112 return-0 岛>`
        // （false 块归共享尾），4.4 对 `if (equip == 0) return 0;` 恒产
        // `jne 继续; mov $0,%eax; jmp` 反形 ⇒ 改正向包裹。
        if (equip != 0)
        {
            if (equip->IsAvatarItem())
            {
                if (equip->get_grade() < best)
                    best = equip->get_grade();
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    if (3 < best)
        return 0;
    return best;
