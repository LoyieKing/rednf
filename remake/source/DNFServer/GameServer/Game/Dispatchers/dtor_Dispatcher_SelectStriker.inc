Dispatcher_SelectStriker::~Dispatcher_SelectStriker() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e4066 orig/ours=180/150 ndiff-blocks=24
//   帧差 sub $3c vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x3c,%esp
// [SweepDispatchers 2026-09-09] **真语义 bug 修复**（149→180 = ORIG 180，p167 18/18 CLEAR）：
//   ① m_field29 是 4 位有符号位域：ORIG 0x081e4262 `movzbl 0x29; shl $4; sar $4,%al; movsbl`
//      旧码 `(short)(char)((raw<<4)>>4)` 用有符号 char 往返恒等于 raw（0x1F 应得 -1，旧得 31）
//      ⇒ 改 `(short)((signed char)((unsigned char)m_field29 << 4) >> 4)`；
//   ② 删 characNo/entry 引用/script 具名缓存（P183）：ORIG 7 次独立 vector::operator[]；
//   ③ 循环/比较改 int idx + `(int)monsterCount > i` 形；IsLoadStrikerSkill 返回 char→bool
//      （ORIG 0x086086aa 直返 bool 字段 + 调用点 0x081e4209 `xor $1;test;je` 无 setcc，
//      SkillSlot.h/.cpp 同步，mangled/布局不变，Main 已批准）。
