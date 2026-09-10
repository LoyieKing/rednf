Manager::~Manager() {}

// ---- ORIG T 0x0813101e（advancealtar__Manager/eplp.md 全文）----
// [b28d-solo-advancealtar_Manager N1 封顶] 第三守卫正向 if（P257）与出口岛/块序
// 逐条一致；唯余 ORIG 0x08131041 `add $0x0,%eax`（4.1 对 `type+EPLP_N0` 未折叠，
// 4.4 前端恒折；探针 /tmp/eplp_probe{,2}.cpp 七形 + 本次 /tmp/b28d_altar_probe/
// eplp3.cpp 五形（type+EPLP_N0 / +0u / (uint)cast / (int)+0 / uint+EPLP_N0）
// 全折，4.4.6 后端亦同）⇒ N1。实测 24 vs 23 条（仅差该 add），语义等价。
