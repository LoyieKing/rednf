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
    register unsigned char itemIdx = static_cast<unsigned char>(info.m_0);
    SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
    info.m_4 =
        skillW->try_refund_skill(itemIdx, level, job, firstGrow, secondGrow, addLevel,
                                 refundCount);
