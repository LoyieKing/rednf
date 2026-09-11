// ORIG 0x081d7f1e（报告 Dispatcher_AchievementTrigger/process.md）：
// msg 为 MSG_ACHIEVEMENT_TRIGGER（m_field0d/11/13/15）；state<=2 → 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d7f1e orig/ours=48/45 ndiff-blocks=9
//   帧差 sub $4c vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
int Dispatcher_AchievementTrigger::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    (void)param;
    // ORIG +0x09 先装载 msg 引用槽 -0x1c（在 get_state 之前）；4 个字段读为实参右值
    // 跨 receiver call 保活（edi/esi/ebx + 溢出槽 -0x2c），非具名局部
    MSG_ACHIEVEMENT_TRIGGER& m = reinterpret_cast<MSG_ACHIEVEMENT_TRIGGER&>(msg);
    if (user->get_state() <= 2)
    {
        return 0;
    }
    ((CAchievement*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xf))
        ->setTrigger((unsigned int)m.m_field0d,
                     (unsigned short)m.m_field11,
                     (unsigned short)m.m_field13,
                     (unsigned short)m.m_field15);
    return 0;
}