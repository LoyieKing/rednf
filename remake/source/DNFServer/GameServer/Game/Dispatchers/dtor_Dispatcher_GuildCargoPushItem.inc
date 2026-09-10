Dispatcher_GuildCargoPushItem::~Dispatcher_GuildCargoPushItem() {}
// ORIG 0x081d0f64（OnlyOrigBatch13 组 E 转写；3 参数符号，字段自 ParamBase 读）：
// state==3 且 curCharac 有效；guildkey==0→100；无公会驻地→0xb5；Secu Check 非 0
// 透传；物品锁→0xd5；param+0x10 与槽内 m_addInfo 不符或 find_item 失败→0xc5。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d0f64 orig/ours=112/113 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x1c(%ebp) | mov    %eax,-0x18(%ebp)
