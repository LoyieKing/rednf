Inter_ReplyGuildCreate::~Inter_ReplyGuildCreate() {}
// ORIG 0x084d368a T dispatch_sig —— 建会应答：成功 → GuildCreate 日志 + 公会代理缓存 +
// 全服基础信息广播；失败 → 退还建会费用并刷新物品列表。
// 报告 docs/class_func_reports/Inter_ReplyGuildCreate/dispatch_sig.md 全文转写。
// msg 布局：+0xa charID(int) +0x12 result(int) +0x16 公会名(0x16)。
