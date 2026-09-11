// ==================== Dispatcher_RequestPrcoomDayilyReward（ORIG 0x81a0612）====================
// dispatch_sig：user==0→0x7e8；getCurCharacR==0→0；state∉[3,4]→0；
//   每日奖励剩余次数（CUser+0x8ec32）<1→SendCmdErrorPacket((ENUM_CMDPACKET)0x259,0x16)；
//   读 int itemId（0x212f/0x2130 限定，错误 0x7fb/0x800）；find_item 后
//   make_item 注入背包（invenType=0xe），成功扣减次数并发
//   InterSelectPcroomDailyReward::SendPacket / DBUpdatePcroomDailyReward::makeRequest。
Dispatcher_RequestPrcoomDayilyReward::~Dispatcher_RequestPrcoomDayilyReward() {}
