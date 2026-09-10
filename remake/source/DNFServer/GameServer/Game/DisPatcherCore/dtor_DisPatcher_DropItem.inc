DisPatcher_DropItem::~DisPatcher_DropItem() {}
// ORIG 0x081c2d9a T _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DropItem/check_error.md 汇编+反编译 C 全文转写）：
// 角色空/数量<1 → -1；state==10 → 0x13；组队血副本 → 0x13；综合PVP频道 → 0x13；
// isRestricted(1,1) → param.m_c=1,0xd1；m_padTail!=1 时安全保护 0x1e（param.m_c=1,返码）；
// CheckItemLock → 0xd5；isHackUser → 0x13；space 1 → 0x17；space 3 且 pos<10 → 0x17；
// [b28d-29] N82/P211：形参副本槽 + store 装载序差，调用点与槽集合全等  (ORIG 0x081c2d9a insn 146->148)
// space 0 且 pos==0 且 count>1000 → 0x46；否则 0。（ORIG +0x18b 存在未读死局部 6）
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 56（ORIG 146 / ours 148），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
