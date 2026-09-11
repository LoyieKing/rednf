// ============================================================================
// D1 依赖前向声明（定义在本 TU 文件尾的 D1 依赖定义区）：
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08095c32 orig/ours=832/811 ndiff-blocks=141
//   帧差 sub $84 vs $94 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x84,%esp | mov    %eax,-0x5c(%ebp) | movl   $0x0,-0x58(%ebp)
int computeHpDamageSDC(STComputeDamageVariable* v, void* arg2, void* defender);  // ORIG T 0x08095c32
