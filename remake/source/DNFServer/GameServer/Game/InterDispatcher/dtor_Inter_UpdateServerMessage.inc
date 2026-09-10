Inter_UpdateServerMessage::~Inter_UpdateServerMessage() {}
// ORIG 0x084da3e6 T dispatch_sig —— 全服公告转发：type 0 → 0xc0 包全频道广播；
// type 1 → Packet_Monitor_Server_Message_Info 上报 monitor 服务器。
// 报告 docs/class_func_reports/Inter_UpdateServerMessage/dispatch_sig.md 全文转写。
// msg 布局：+4 sourceChannel(int) +8 kind(u8) +9 index(u8) +0xc msgLen(int) +0x10 msg
//           +0x44 nameLen(int) +0x48 name +0x5c time(int)。
