DisPatcher_ResPeer::~DisPatcher_ResPeer() {}
// ============================================================================
// ORIG T 0x081f088e _ZN18DisPatcher_ResPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ResPeer/dispatch_sig.md，2061 insn）
// state==3 门禁 → 三字段（peerIdx/msg+0xd、requestType/msg+0xf、cookie/msg+0x10）
// → GetReceivedRequestType 匹配校验（5/7 与 6/8 成对）→ 按 requestType 分派：
//   0=组队邀请应答  1=交易邀请应答  2=PvP 房间加入  3=战争房间加入
//   5/7 与 6/8=expert_job 应答（两侧参数互换）。错误包形态：
//   put_header(1,0xb)/put_byte(0)/put_byte(code)/put_byte(tail)/finalize(true)。
// 汇编中 GetAssaultPrevent 以两参栈式调用（this 经 ecx），沿用
// W6_AssaultMgrDeps.cpp 同款成员函数指针 ABI 转接。
// [b28d-29] P30/N4 + N82：帧 0xec vs 0xf0 + ORIG 两步 add 寻址 vs 4.4 独立局部槽；语义集合全等  (ORIG 0x081f088e insn 2061->2029)
// ============================================================================
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 2844（ORIG 2055 / ours 2023），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
