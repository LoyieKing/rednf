// ==================== Dispatcher_GetAchievementReward（ORIG 0x8140c74）====================
// dispatch_sig：校验失败返回 0x571；read(0x14) → process（ORIG 为空）→
//   send_packet(0x243)。send_packet：put_byte(1)+put_binary(res,0xc)+
//   put_int(count)+逐项 _RewardItem（sendAchievementReward 未建模，推断）。
Dispatcher_GetAchievementReward::~Dispatcher_GetAchievementReward() {}
