Inter_SecurityCardIssue::~Inter_SecurityCardIssue() {}
// ORIG 0x084d3a22 T dispatch_sig —— 安全卡发放：加密序列号/账号/答案串并下发 0xa4 包，
// 本地初始化 CSecurityCard。msg 布局：+4 serial +0x25 answers +0x37 serial 原文
// +0x5e answers 原文 +0x8c 错误码(int)。
// 报告 docs/class_func_reports/Inter_SecurityCardIssue/dispatch_sig.md 全文转写。
