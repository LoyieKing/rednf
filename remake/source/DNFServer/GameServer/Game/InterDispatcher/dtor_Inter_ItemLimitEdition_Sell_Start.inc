Inter_ItemLimitEdition_Sell_Start::~Inter_ItemLimitEdition_Sell_Start() {}
// ORIG 0x084db878 T dispatch_sig —— 限购版道具开卖：清旧登记（广播移除包）、逐条目
// 注册、广播信息包。msg 布局：+0xa flag(u8) +0xb count(u32) +0xf 起条目 stride 0x48。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Sell_Start/dispatch_sig.md 全文转写。
