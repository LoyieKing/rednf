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
    SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
    info.m_4 =
        skillW->refund_skill(static_cast<unsigned char>(info.m_0), level, job,
                             firstGrow, secondGrow, addLevel, refundCount);
    if (info.m_4 >= 0)
    {
        SkillSlot* skillR = (SkillSlot*)(user->getCurCharacSkillR());
        info.m_8 = skillR->get_skill_level(info.m_4);
    }
