Inter_DeleteCharac::~Inter_DeleteCharac() {}
// ORIG 0x084bf810 T dispatch_sig —— 删除角色应答分发：结果码 1/2/3 直接回执，
// 其余构造 Packet_Monitor_Charac_Delete 上报 monitor/guild 后本地删角色并刷基础信息。
// 报告 docs/class_func_reports/Inter_DeleteCharac/dispatch_sig.md 全文转写。
// msg 布局：+4 charNo(int) +8 result(int) +0xc guildKey(int) +0x10 eventCharacFlag(char)。
