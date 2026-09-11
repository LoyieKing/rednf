// ============================================================================
// ClearDungeonMission::processMission —— ORIG T 0x081653b4（processMission.md）。
// 清图计数推进：m_extra[0] 为上限；missionType==2 且 field8==7 的任务不推进
// （特殊关卡类型屏蔽）。
// ============================================================================
void ClearDungeonMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                         unsigned int param)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x081653b4：条件 1 `limit <= field4`
    // （cmp %ax,%dx; setbe; jne 跳尾岛）；条件 2 短路结果整体 bool 物化
    // （mov $1/jmp/mov $0/test/jne）；条件 3 `limit <= field4+param` 钳制 fall-through。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    if (getMissionType() == 2 && value.m_field8 != 7)
        return;
    if (m_condition.m_extra[0] <=
        static_cast<unsigned int>(value.m_field4) + param)
    {
        value.m_field4 = m_condition.m_extra[0];
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(param);
    }
}