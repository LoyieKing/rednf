DisPatcher_MoveItem::~DisPatcher_MoveItem() {}
// ORIG 0x081c57a0 T _ZN19DisPatcher_MoveItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_MoveItem/check_error.md 汇编+反编译 C 全文转写）：
// m_field14<0 → -1。toSpace(space)>5 时仅 ==0xe、<=5 时 ==4/5 或 ==3 走限制/锁定检查；
// ==4/5 或 0xe：CheckItemTradeLimitCountToMoveByUser 假 → 0x72；==3 且 fromSpace==7：
// GetInvenSlot(3,fromPos) 非空且 fromPos>0x8b → 需锁定检查；需检查者 CheckItemLock
// [b28d-29] P251：ORIG 0x5/0xe/0x3 值域为稀疏 switch 决策树，源码为 if 链；槽往返随 P0 属 N82 族  (ORIG 0x081c57a0 insn 108->103)
// 命中 → 0xd5；否则 0。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 107 / ours 102），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
