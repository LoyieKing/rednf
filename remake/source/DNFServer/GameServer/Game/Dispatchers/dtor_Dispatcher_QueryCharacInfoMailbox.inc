Dispatcher_QueryCharacInfoMailbox::~Dispatcher_QueryCharacInfoMailbox() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08226bda orig/ours=123/93 ndiff-blocks=10
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
// [SweepDispatchers 2026-09-09] 真缺口修复（93→123 = ORIG 123；p167 13/13 CLEAR）：
//   ① ORIG 0x08226d36 只调一次 trim 且其返回值直接作 strncmp 第 1 参
//      （`call trim; mov %eax,(%esp)`）——旧码 `trim(name); strncmp(name,…)` 双求值；
//   ② nameLen=0 先于 name 清零（ORIG 0x08226c3a/0x08226c41）+ name 改 `= {0}`。
