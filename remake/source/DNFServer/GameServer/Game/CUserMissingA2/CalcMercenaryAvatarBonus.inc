// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868ea68 sz 0x141。雇佣兵 avatar 等级加成 = 全身 avatar 装备的最低
// 品阶：初值 10；getCurCharacR/getCurCharacInvenR 为空→0；否则 i∈[0,7] 遍历
// GetInvenRef(INVEN, i)：空引用/find_item 失败/dynamic_cast<CEquipItem*> 失败/
// IsAvatarItem()==0（vtable+0x10 槽 4，两处 dtor 后第 5 槽）任一即返 0；
// 否则取 min(get_grade)；循环后 >3 归零。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0868ea68；p167 CLEAR（8/8），残差为帧/槽号与
// callee-saved 分配（4.1 保守帧 vs 4.4 紧凑帧），语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒NEAR：去 `CItem* item` 具名局部（dynamic_cast
// 直调 find_item 结果，ORIG 0x0868eb0f 无 item 槽）+ P222 正向包裹 equip 判定 +
// 尾部 `if (3 < best) return 0;` ⇒ 槽位集与 ORIG 全等（best/-0x1c、inven/-0x18、
// i/-0x14、ref/-0x10、equip/-0xc）。残差仅 N7（虚调用目标 4.4.7 恒 %eax vs ORIG
// %edx）+ .o 重定位伪差；p167 CLEAR 8/8。
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
}