Timer_CheckAuctionReady::~Timer_CheckAuctionReady() {}

// ---------------------------------------------------------------------------
// dispatchPacket 只读布局视图：这些上游包类的权威布局在 Guild/Monitor 域头
// （DNFServer/GameServer/{Guild,Monitor}/*.h）里，与本 TU 已 include 的 Game 域
// 头存在同名类冲突（如 hyperlink_item_info / STBlackUserDBType / CPacketTranslater
// 重定义，实测编译失败），故沿用本文件 GAxxxView 先例：pack(1) 局部视图，字段
// 名/类型/偏移逐字抄自权威头（证据见各注释），ORIG 指令为 `mov 0xN(%reg)` 4B 整字
// 读取（docs/class_func_reports/CDispatchServerPacket/dispatchPacket.md）。
// ---------------------------------------------------------------------------
