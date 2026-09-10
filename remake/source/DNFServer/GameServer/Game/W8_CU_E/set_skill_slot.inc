void CUser::set_skill_slot(char* data1, char* data2, int size)
{
    // P0：ORIG 无 slot 槽，getCurCharacSkillW 结果直传（%eax→(%esp)）。
    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot(data1, data2, size);
