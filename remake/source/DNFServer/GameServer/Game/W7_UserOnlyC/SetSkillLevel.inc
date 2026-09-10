bool CUser::SetSkillLevel(int skillIdx, int level)
{
    // b28d-52：ORIG 0x0867b528 `movzbl 0x711d8(%eax); xor $1; test; jne` =
    // `!m_gmUserFlag`（P248② bool 字段 + 裸 !field；该偏移 = CUser+0x711d8，
    // CUser+0x705dc 的 m_clientSpec）。原裸偏移 `!= 1` 违反 §4.3 且产 cmp $1/jne。
    // 短路汇聚形（P214①）：`if (!flag || getCurCharacR() == 0) return 0;`
    if (!m_gmUserFlag || getCurCharacR() == 0)
        return 0;

    // P0/P36：ORIG job 无槽（%ebx 跨 G_CDataManager 保活）⇒ 内联实参。
    CSkill* skill = G_CDataManager()->find_skill(get_charac_job(), skillIdx);
    if (skill == 0)
        return 0;

    // P0：ORIG 0x0867b5a2 `test %al,%al; setle` 无 secondGrow 槽（两次独立调用）。
    if (getCurCharSecondGrowType() < 1)
    {
        level = std::max(
            0, std::min(skill->get_type_max_level(getCurCharFirstGrowType()),
                        level));
    }
    else
    {
        int maxLevel = skill->get_second_growtype_max_level(
            getCurCharFirstGrowType(), getCurCharSecondGrowType());
        if (maxLevel == 0)
            maxLevel = skill->get_type_max_level(getCurCharFirstGrowType());
        level = std::max(0, std::min(maxLevel, level));
    }

    // P0：ORIG treeIdx 无槽（%esi 跨调用保活）⇒ 内联实参（右起求值序与 ORIG 一致）。
    reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->growtype_skill(
        get_charac_job(), skillIdx, level,
        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
    return 1;
