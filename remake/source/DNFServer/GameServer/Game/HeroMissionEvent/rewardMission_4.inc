// ============================================================================
// AttendanceMission::rewardMission —— ORIG T 0x081661d6（rewardMission.md）。
// 骨架同 UseFatigueMission::rewardMission；已奖励分支无日志，条件不满足分支
// 用 cMyTrace（0x1a4, lvl 5）而非 logFormat。
// ============================================================================
bool AttendanceMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        // [b28d-solo-HeroMissionEvent] ORIG 0x081661d6 先求值 varargs（edi/esi/ebx）
        // 再 call cMyTrace::C1，最后 call operator()（P227 全表达式临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1a4, 5)
            ("[HeroMission] Reward condition error. (charac:%u,mission:%d,count:%d)",
             user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}