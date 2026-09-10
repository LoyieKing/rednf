Dispatcher_AchievementTrigger::~Dispatcher_AchievementTrigger() {}
// ORIG 0x081d7f1e（报告 Dispatcher_AchievementTrigger/process.md）：
// msg 为 MSG_ACHIEVEMENT_TRIGGER（m_field0d/11/13/15）；state<=2 → 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d7f1e orig/ours=48/45 ndiff-blocks=9
//   帧差 sub $4c vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
