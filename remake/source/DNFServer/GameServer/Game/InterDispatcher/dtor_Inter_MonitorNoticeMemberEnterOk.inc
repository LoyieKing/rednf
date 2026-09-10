Inter_MonitorNoticeMemberEnterOk::~Inter_MonitorNoticeMemberEnterOk() {}
// ORIG 0x084cd218 T dispatch_sig —— 会员进入应答分发：solo 拒绝 / 上线会员设置 / 0x4a 状态包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberEnterOk/dispatch_sig.md 全文转写。
// msg 布局：+0xa result(byte) +0xb sub(byte) +0xc flag(byte) +0x11 charID(int)
//           +0x15 u8 +0x16 字符串 +0x34 u8 +0x35 uint。
