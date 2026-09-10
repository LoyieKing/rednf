Inter_MonitorPayTaxToUpper::~Inter_MonitorPayTaxToUpper() {}
// ORIG 0x084ce164 T dispatch_sig —— 会员税额金币入账 + 会员奖励疲劳发放（上限钳制）。
// 报告 docs/class_func_reports/Inter_MonitorPayTaxToUpper/dispatch_sig.md 全文转写。
// msg 布局：+0xe charID(int) +0x12 税额金币(int) +0x16 奖励疲劳(uint) +0x1a 字符串。
