#ifndef GAME_DBINSERTGIVEBOXEVENTREWARD_H_
#define GAME_DBINSERTGIVEBOXEVENTREWARD_H_

class CUser;

// df_game_r 还原 —— DBInsertGiveBoxEventReward（礼盒事件奖励 DB 请求，
// ORIG 为无命名空间类，本仓库以同 mangled 命名空间承载）。
// MSG 定义见 MoreTypes.h；makeRequest ORIG 0x0815d2c8
// （_ZN26DBInsertGiveBoxEventReward11makeRequestEP5CUser）；dispatch ORIG
// 0x0815d3ee（_ZN26DBInsertGiveBoxEventReward8dispatchEiiP6Stream，待后续批次）。
namespace DBInsertGiveBoxEventReward
{
void makeRequest(CUser* user);
}

#endif  // GAME_DBINSERTGIVEBOXEVENTREWARD_H_
