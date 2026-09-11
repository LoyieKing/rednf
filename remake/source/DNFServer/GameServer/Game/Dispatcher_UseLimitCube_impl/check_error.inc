// ============================================================================
// check_error —— ORIG 0x081d3bbc
// 参数：(slotA, itemIdx, slotB, invA, invB, cube)
// 校验：
//   cube == 0                     → 0x11
//   invA.m_addInfo == 0 || != itemIdx → 0x11
//   invB.m_addInfo == 0           → 0x11
//   !cube->is_stackable()         → 0x11
//   cube->GetItemType() != 0x1d   → 0x11
//   !cube->is_included_material_for_limit_cube(itemIdx) → 0x11
//   否则 0
// slotA/slotB 仅参与签名（check_error 内部未使用，见汇编 0x081d3bbc..0x081d3c61）。
// ============================================================================
// [b28c-31] N7：虚调用 callee ORIG %edx vs 4.4 %eax（0x081d3c2x vptr+0xc 槽），
// 其余 57=57 全等（仅 call 目标重定位显示差）。
int Dispatcher_UseLimitCube::check_error(int slotA, unsigned int itemIdx, int slotB,
                                         Inven_Item& invA, Inven_Item& invB,
                                         const CItem* cube)
{
    (void)slotA;
    (void)slotB;
    if (cube == 0)
    {
        return 0x11;
    }
    if (invA.m_addInfo == 0 || (unsigned int)invA.m_addInfo != itemIdx)
    {
        return 0x11;
    }
    if (invB.m_addInfo == 0)
    {
        return 0x11;
    }
    if (!cube->is_stackable())
    {
        return 0x11;
    }
    if (cube->GetItemType() != 0x1d)
    {
        return 0x11;
    }
    if (!((CStackableItem*)cube)->is_included_material_for_limit_cube(itemIdx))
    {
        return 0x11;
    }
    return 0;
}