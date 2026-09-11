// ORIG 0x082864fc：find_skill(job, slot)==0 → false；
// 首一转类型命中 / 二转上限>0 / IsStealable(first, second) 任一成立 → true。
bool CHackLog_InvalidUsingSkill::IsValidUsingSkill(const SKILL_USING_VALIDATE_INFO* info)

{
    CSkill* skill = G_CDataManager()->find_skill(info->m_job, (int)info->m_slot);
    // ORIG 0x081c... 头块 `cmpl $0x0,-0xc; je 远端 false 尾` = 正向嵌套 if（probe p8 fb 实证）
    if (skill != 0)
    {
    if (IsSkillMatchFirstGrowType(skill, info->m_firstGrowType))
    {
        return true;
    }
    if (IsSkillMatchSecondGrowType(skill, info->m_firstGrowType, info->m_secondGrowType))
    {
        return true;
    }
        // ORIG 尾块：`call; test %al,%al; je L_false; mov $1,%eax; jmp; L_false: mov $0,%eax`
    if (skill->IsStealable((int)info->m_firstGrowType, (int)info->m_secondGrowType))
    {
        return true;
    }
    }
    return false;
}
