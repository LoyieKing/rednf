// [b28d-18 N82] p167 CLEAR（36/36）；残差=槽位分配（ORIG -0x44/-0x48/-0x50 vs 4.4 -0x30/-0x18/-0x20） | ORIG 0x0829dd82
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（36/36）；残差=N82 槽位/寄存器 + N89 STL 展开。
// [K8GlobalData 2026-09-11 · §6.5 串盲区 + P245/P281 反馈] ORIG prett 实串
// `static bool GlobalData::initTournamentDB(const char*)`（符号
// _ZZN10GlobalData16initTournamentDBEPKcE19__PRETTY_FUNCTION__@0x08c1b100）；ORIG
// 0x0829dddb `mov 0x8(%ebp),%eax; mov %eax,0x4(%esp)` 直证 0x8(%ebp) 已是第 1 实参
// （无 this）⇒ ORIG 侧是**类静态成员**。**父泳道裁决：本轮不落（N）** —— 本仓
// GlobalData 为 namespace 形态（类化重构超范围）；在 namespace 内加 `static` 会变
// 内部链接（_ZL… 与 ORIG 不符、TU 外不可见），成本远超收益（仅日志文本）⇒
// prett 串差 `static` 字样 = namespace 形态固有；槽位残差按原 N82/N89 结论封顶。
bool GlobalData::initTournamentDB(const char* fileName)
