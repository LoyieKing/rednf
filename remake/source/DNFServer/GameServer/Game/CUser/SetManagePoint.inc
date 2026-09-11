// ============================================================================
// ORIG T 0x0868fe72 _ZN5CUser14SetManagePointEi：点数封顶升级所需后写入。
// ============================================================================
void CUser::SetManagePoint(int point)
{
    // ORIG 0x868fe79-0x868fe9f：两次 G_CDataManager call（不折叠）；lvmax 先落
    // -0x10 槽再 `mov -0x10,%ebx`（跨 call callee-saved 持值，技巧 2/31），实参
    // `mov %ebx,0x4(%esp)` 先于 `mov %eax,(%esp)`。
    // b28d-solo-CUser N封顶: ORIG 0x868fe79 值落 -0x10 槽再 `mov -0x10,%ebx` 跨 call
    //   持值（push ebx/sub $0x24）；4.4 三形实测：具名→双槽(-0x14/-0x10)多中转、
    //   裸 register→直入 %ebx 免槽（无 push ebx）、语句表达式→值内联更差 ⇒ N7/N18a
    //   寄存器分配族（register 形为最近形 our=26 vs orig=27）。
    int lvmax =
        G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    int require = G_CDataManager()->m_altarConfig.m_characManageScript
                      ->GetLevelUpRequirePoint(lvmax);
    if (point > require)
        point = require;
    m_field8ebd4 = point;
}