Inter_AuctionResultAskRegistedItemNum::~Inter_AuctionResultAskRegistedItemNum() {}
// ============================================================================
// ORIG 0x084d5930 T dispatch_sig —— 拍卖行"已登记物品数查询应答（登记执行）"。
// 报告 docs/class_func_reports/Inter_AuctionResultAskRegistedItemNum/dispatch_sig.md
// 全文转写（1073 insn，Gold 与 Cera 双通道）。包布局 PCK_AUCTION_REGIST_ITEM_GA
// 依 ORIG ctor @0x84e9a12 / DnfItemInfo::reset @0x84e9094 逐条 store 还原；
// ROI_Category::_sort @0x0822ac30。gmList ORIG d @0x093701d8 实测五值。
// ============================================================================

// ---- ORIG 辅助类型（同源弱符号；Game 侧唯一声明点在本 TU）----
