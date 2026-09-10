DisPatcher_ReqPeer::~DisPatcher_ReqPeer() {}
// ============================================================================
// ORIG T 0x081eed08 _ZN18DisPatcher_ReqPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReqPeer/dispatch_sig.md，1379 insn）
// state∈{3,6,8} 门禁 → 三字段读取 → 发起方门禁（私摊/分解店/幽灵/整合 PvP）→
// target 查找与 solo-play/游戏选项拒绝 → check_party_teleport_user(reqType,
// user,target,true) → 按 reqType 分派：0=组队、1=交易、2=PvP 房间、
// 3=战争房间、4=袭击请求、5/7 与 6/8=expert_job。成功尾部统一
// SetReceivedRequestType(target, reqType)。MSG_REQ_TO_PEER 仅作 SoloPlay
// 拒绝包上下文（类型未建模，零缓冲视图【推断】）。
// §6.2 巨件核销：P167 194/194 [0缺0多]；gap 1379→1379/1387；残差归因 N82/P112
// （4.1 不复用槽 vs 4.4 复用 + 块排布，diff 形态计数对称无缺类，详见
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-29] P30/N4 + N82：ORIG 结构体基址 lea -0x50 + add $0xd/0xf/0x10 两步，4.4 折独立 lea；语义集合全等  (ORIG 0x081eed08 insn 1379->1387)
// ============================================================================
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 686（ORIG 1377 / ours 1385），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
