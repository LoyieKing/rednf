void DisPatcher_BuySkill::try_refund_skills_process(CUser* user, char /*treeKind*/,
                                                    buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());          // -0x30
    // [b28d-solo-DisPatcherCore] P248 三条件（跨 call 保活 + 4.4 不自升格 +
    // ORIG 目标为 callee-saved %esi/%ebx）⇒ 保留裸 register（非 asm 绑定，P0 合规）。
    register int job = user->get_charac_job();               // ORIG esi
    register int level = user->get_charac_level();           // ORIG ebx
    // ORIG `mov 0x14(%ebp),%eax; movzbl (%eax),%eax; movzbl %al,%edi`：info.m_0
    // 在 skillW 的 call 之前取值并跨该 call 保活于 %edi ⇒ 具名 register 局部。
    // [FL4 负结论] P267 两段装载不可造：源侧改 `(unsigned char)(unsigned int)info.m_0`
    // 实测 4.4 仍发单段 `movzbl (%eax),%edi`（ORIG 为两段 cs 32/33），帧 0x5c 不变
    // ⇒ 归 N18a（4.1 冗余 regmove 折半不可造）族，维持单段 cast 原形。
    register unsigned char itemIdx = static_cast<unsigned char>(info.m_0);
    // [FL4] P0/P89：ORIG 0x...`call getCurCharacSkillW; mov %eax,%edx`（receiver 直落
    // %edx，**无**具名指针槽）；旧形 `SkillSlot* skillW = ...` 占 -0x1c 槽 + 回读 2 条。
    info.m_4 =
        ((SkillSlot*)(user->getCurCharacSkillW()))
            ->try_refund_skill(itemIdx, level, job, firstGrow, secondGrow, addLevel,
                               refundCount);
