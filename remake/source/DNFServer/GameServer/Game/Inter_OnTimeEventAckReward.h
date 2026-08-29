#ifndef GAME_INTER_ONTIMEEVENTACKREWARD_H_
#define GAME_INTER_ONTIMEEVENTACKREWARD_H_

// df_game_r 还原 —— Inter_OnTimeEventAckReward（整点事件奖励 ACK 分发器）。
// ORIG 符号：
//   _ZN26Inter_OnTimeEventAckReward12dispatch_sigEP5CUserPci    T 0x084e11ec
//   _ZN26Inter_OnTimeEventAckReward22SendOnTimeRewardByMailEjjj T 0x084e1578
//   ~D1 0x084f6ff0 / ~D0 0x084f7020（编译器生成；vtable@0x8c8eb00
//   槽序 dispatch_sig / ~D1 / ~D0；D1 重置 vptr 后调
//   ISigDispatcher::~ISigDispatcher，见 0x84f6ff0）
#include "InterDispatcher.h"

class Inter_OnTimeEventAckReward : public ISigDispatcher
{
public:
    virtual int dispatch_sig(CUser* user, char* data, int len);
    void SendOnTimeRewardByMail(unsigned int nCharacNo, unsigned int itemNo, unsigned int count);
};

#endif  // GAME_INTER_ONTIMEEVENTACKREWARD_H_
