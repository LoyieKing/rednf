int CUser::get_skill_count()
{
    // ORIG 0x0866af86：getCurCharacSkillR 结果直接内联为链式 receiver（P0/P24），
    // 具名局部 slot 触发 -0xc 槽往返 + 帧 0x28 扩张。
    return ((SkillSlot*)getCurCharacSkillR())->get_skill_count();
