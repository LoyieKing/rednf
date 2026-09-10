Dispatcher_UpgradeItem::~Dispatcher_UpgradeItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082006d0 orig/ours=258/226 ndiff-blocks=24
//   帧差 sub $150 vs $d8 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0x150,%esp
// [SweepDispatchers 2026-09-09] 真缺口修复：ORIG 0x0820082c 调 get_int(unsigned long&)（Rm）
//   而旧码对 unsigned int 字段选 Rj ⇒ p167 MISSING 1；调用点改
//   reinterpret_cast<unsigned long&>(req.m_targetItemIdx) 后 CLEAR（残余仅 C1/C2 同址别名
//   P93 + 我方多 1 次 memset：ctor 内已 memset(m_28,0,0x81)，此处为重复清零，待属主裁量）。
