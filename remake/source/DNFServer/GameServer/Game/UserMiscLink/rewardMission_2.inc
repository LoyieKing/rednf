bool TotalMission::rewardMission(CUser* user, HeroMissionValue& value,
                                 std::vector<HeroMissionValue>& values)
{
    // [b28d-solo-UserMiscLink] ORIG 0x081663fe。真语义修复：条件为
    // !isRepeatMission() && value.m_fieldA != 0（旧码误写 ||，重复任务被误拒）。
    // && 的 4.1 bool 物化岛（je L_false; je L_false; mov $1; jmp; mov $0; test）
    // 由 P226 语句表达式唯一复现（4.4 裸 && 恒折直跳）。
    if (({ !isRepeatMission() && value.m_fieldA != 0; }))
    {
        return false;
    }

    // 全部子任务（m_mission==6 或已完成）均达成才发奖。ORIG 走迭代器
    // begin/end/ne/ptEv/ppEv（-0x14 it / -0x10 end），非下标 operator[]。
    bool allDone = true;
    std::vector<HeroMissionValue>::iterator it = values.begin();
    while (it != values.end())
    {
        if (({ it->m_mission != 6 && it->m_fieldA == 0; }))
        {
            allDone = false;
            break;
        }
        ++it;
    }

    // ORIG 0x081664c9：ok 预清 0 + cmpb $0,allDone 直比跳尾（P222 正向 if 包裹）。
    bool ok = false;
    if (allDone)
    {
        ok = rewardProcess(user);
        value.m_fieldA = (unsigned short)(value.m_fieldA + 1);
        value.m_field6 = 1;
        if (isRepeatMission())
        {
            value.reset();
        }
    }
    return ok;
}