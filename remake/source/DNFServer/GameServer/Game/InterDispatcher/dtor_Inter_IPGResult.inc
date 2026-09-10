Inter_IPGResult::~Inter_IPGResult() {}
// ORIG 0x084ceaee T dispatch_sig —— IPG 计费结果分发：数据流分支（ProcessIPGUserData）
// 与查询结果分支（SendCashData / 点数查询 / CERA 查询 / 错误状态包）。
// 报告 docs/class_func_reports/Inter_IPGResult/dispatch_sig.md 全文转写。
// msg 布局：+0 isData(char)；数据分支 data+1 req / data+0x2dd res；查询分支 data+1 起。
