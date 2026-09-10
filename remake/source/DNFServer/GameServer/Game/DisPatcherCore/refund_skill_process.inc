void DisPatcher_BuySkill::refund_skill_process(CUser* user, char /*treeKind*/,

                                               buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());         // -0x30
    // [b28d-solo-DisPatcherCore] P248 三条件（跨 call 保活 + 4.4 不自升格 +
    // ORIG 目标为 callee-saved %esi/%ebx）⇒ 保留裸 register（非 asm 绑定，P0 合规）。
    register int job = user->get_charac_job();               // ORIG esi
    register int level = user->get_charac_level();           // ORIG ebx
    // [G5-MiscC] P0/P89：ORIG 0x081bfcca `call getCurCharacSkillW; mov %eax,%edx`
    // （receiver 直落 %edx，**无**具名指针槽；我方原 `SkillSlot* skillW` 多占一槽
    // + 回读两条）⇒ 内联回 receiver 位。ORIG 实参布局同序：
    // &refundCount(0x1c) / addLevel / secondGrow / firstGrow / job(%esi) /
    // level(%ebx) / info.m_0(%edi) / this(%edx)，全部从右到左求值。
    info.m_4 = ((SkillSlot*)(user->getCurCharacSkillW()))
                   ->refund_skill(static_cast<unsigned char>(info.m_0), level, job,
                                  firstGrow, secondGrow, addLevel, refundCount);
    if (info.m_4 >= 0)
    {
        // [G5-MiscC] P0/P89：ORIG 0x081bfd1d `call getCurCharacSkillR;
        // mov %eax,(%esp)` 后直读 `mov 0x4(%eax),%ebx`(=info.m_4) 传参，结果
        // 直接 `mov %eax,0x8(%edx)` 回写 —— 无 skillR 具名槽 ⇒ 内联链式。
        info.m_8 = ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4);
    }
}
