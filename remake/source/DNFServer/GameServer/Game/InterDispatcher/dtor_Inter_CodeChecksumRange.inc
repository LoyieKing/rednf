Inter_CodeChecksumRange::~Inter_CodeChecksumRange() {}
// ORIG 0x084d0648 T dispatch_sig —— 校验码区间比对：对所有登录用户执行校验并计数作弊。
// 报告 docs/class_func_reports/Inter_CodeChecksumRange/dispatch_sig.md 全文转写。
// [b28d-PoolInterDisp] 收口：getUserByAccIdMap 发明 getter 已删（ORIG 无该符号）；
// 本类经 CGameManager.h `friend class Inter_CodeChecksumRange` 直访 m_userByAccId，
// 复现 ORIG 0x084d069c `call G_CGameManager; lea 0x22c(%eax)` 无 call 形。
