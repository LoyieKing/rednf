Inter_Antibot_SendBufferClient::~Inter_Antibot_SendBufferClient() {}
// ORIG 0x084e43bc T dispatch_sig —— ANTIBOT/DPROTO 缓冲透传客户端：getUser 状态门、
// 异常计数日志、0x230/0x231 包发送与历史记录。
// 报告 docs/class_func_reports/Inter_Antibot_SendBufferClient/dispatch_sig.md 全文转写。
// msg 布局：+0 kind(int,0=ANTIBOT/1=DPROTO) +4 qqID(uint) +8 size(u16) +0xc payload。
