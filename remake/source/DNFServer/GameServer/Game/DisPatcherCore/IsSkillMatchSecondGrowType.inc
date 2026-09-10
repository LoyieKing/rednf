bool CHackLog_InvalidUsingSkill::IsSkillMatchSecondGrowType(const CSkill* skill,
                                                            char a, char b)
{
    // ORIG 0x08286612：setg 后再 `test;je;mov 1;jmp;mov 0`（true/false 分支物化）。
    if (skill->get_second_growtype_max_level((int)a, (int)b) > 0)
        return true;
    return false;
