bool SkillSlot::check_skill_list(
    int job, std::vector<std::pair<int, int> > list, int level) const
{
    // [b28d-solo-SkillSlot] 修法（AE 107=107）：P292 早返 + P28 it++ + P135 右值赋值
    // + P50 迭代器声明即初始化（`iterator it = list.begin();` 直构进 it 槽，
    // 消除默认构造弱实例 C1Ev 与 begin sret 临时槽；帧 0x68→0x58）。
    // ORIG 0x08604c02 形 = `size(); test; sete; test; je` 后 `mov $1; jmp`（空表早返）、
    // 增量段 `ppEi`（后置 ++ 带 sret 临时 -0x1c）、`pair = pair&&`（aSEOS0_ 调用，
    // 临时 pair -0x24/-0x20）、`call check_skill_mastered; xor $1,%eax; test; je`
    // （callee 返回 bool，见头声明）。
    if (list.size() == 0)
    {
        return 1;
    }
    CSkill* skill = 0;
    std::vector<std::pair<int, int> >::iterator it = list.begin();
    std::pair<int, int> pair;
    for (; it != list.end(); it++)
    {
        pair = std::pair<int, int>(*it);
        int skillIdx = pair.first;
        int reqLevel = pair.second;
        skill = G_CDataManager()->find_skill(job, skillIdx);
        if (skill == 0)
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0x42b, "G_CDataManager()->find_skill( %d, %d )", job,
                skillIdx);
            return 0;
        }
        int group = skill->get_group();
        reqLevel = reqLevel - level;
        if (reqLevel < 1)
        {
            reqLevel = 1;
        }
        if (!check_skill_mastered(skillIdx, reqLevel, group))
        {
            return 0;
        }
    }
    return 1;
