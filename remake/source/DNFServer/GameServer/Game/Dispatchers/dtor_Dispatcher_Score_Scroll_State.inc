Dispatcher_Score_Scroll_State::~Dispatcher_Score_Scroll_State() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ff344 orig/ours=117/120 ndiff-blocks=22
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | movswl %ax,%ebx | mov    %ebx,0x4(%esp)
