DisPatcher_SetPartyInfo::~DisPatcher_SetPartyInfo() {}
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮：返回 unsigned（与 int 字段异型，
// 复现 process 调用点 RHS-first+edx 中转）；返回类型不入 mangled/ABI。
