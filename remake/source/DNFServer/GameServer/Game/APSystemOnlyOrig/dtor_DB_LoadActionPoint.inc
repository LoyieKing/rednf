DB_LoadActionPoint::~DB_LoadActionPoint() {}

// ============================================================================
// DB_UpdateActionPoint
// ============================================================================

// MakeActionClearStateString —— 报告全文转写。ORIG T 0x081246d6。
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x081246d6；ORIG 多 std::string 临时
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；137=137；P157 去 rec 引用缓存 + groupIndex vararg 走 unsigned 两段中转后全等）。
// 构造/析构（_ZNSsC1Ev/D1Ev×2）+ _Unwind_Resume，我方缺临时对象与 EH 清理路径。
