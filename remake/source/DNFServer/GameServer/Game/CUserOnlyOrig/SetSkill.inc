bool CUser::SetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo)
{
    int   sp1   = skill->m_remainSpTree1;                   // +0x8
    char* bufA  = reinterpret_cast<char*>(&skill->m_skillSlotTree1[0]);   // +0xc
    int   size  = 0x198;   // ORIG 0x0864db3b movl $0x198,-0x2c（4.1 sizeof*SIZE 常量物化槽）
    int   sp2   = skill->m_remainSpTree2;                   // +0x1a4
    char* bufB  = reinterpret_cast<char*>(&skill->m_skillSlotTree2[0]);   // +0x1a8
    int   tail1 = skill->m_remainSfpTree1;                  // +0x340
    int   tail2 = skill->m_remainSfpTree2;                  // +0x344（ORIG 实参第 4 = sfp2）

    // ORIG 0x0864db71：恒假防御检查（4.1 保留代码）——`cmpl $0x198,-0x2c; je`，
    // 不匹配则 getCurCharacName 结果直落 0x14(%esp) 走 LogManager::logFormat 后返 0
    if (size != 0x198)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x13a6,
            "User %s - size != sizeof( _Mastered_skill ) * _Mastered_skill::SIZE_OF_SKILLSLOT",
            getCurCharacName());
        return false;
    }
    if (getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x13ad, 5);
        trace("CUser::SetSkill User  m_selected is null");
        return false;
    }

    // asm 实参序:(sp1, sp2, tail1, tail2=[0x344])——ORIG 第 4 参为 0x344 字段
    // [DualCUOO 2026-09-09] 实测链式调用（ORIG 形态）反而把 gap 从 6 拉到 10
    // （槽序/寄存器分配连带），故保留具名槽形；余 6 条为 ORIG 把 sp1/sp2/tail1
    // 常驻 ebx/esi/edi（0x0864dc19-0x0864dc1f 加载、0x0864dcec-0x0864dcee 弹出）。
    SkillSlot* slot = (SkillSlot*)getCurCharacSkillW();
    slot->set_remain_sp(sp1, sp2, tail1, tail2);

    slot = (SkillSlot*)getCurCharacSkillW();
    slot->set_skill_slot(bufA, bufB, 0x198);

    setSkillCommandData(reinterpret_cast<char*>(&skill->m_commands[0]));  // +0x348

    // 技巧37：call 结果免槽直落 this 实参（ORIG 0x0864dc87-0x0864dc9d 常参
    // `movl $0x0,0x4(%esp)` 不破 %eax，无 -0xc spill）
    ((SkillSlot*)getCurCharacSkillW())->SetLoadStrikerSkill(false);

    if (get_charac_job() == 9)
    {
        ((SkillSlot*)getCurCharacSkillW())->loadComboSkill(combo);
        ((SkillSlot*)getCurCharacSkillW())->verifyComboSkill();   // ORIG 0x0864dcd1 免槽
    }
    return true;
