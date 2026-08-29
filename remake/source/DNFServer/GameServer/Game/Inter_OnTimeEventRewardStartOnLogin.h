#ifndef GAME_INTER_ONTIMEEVENTREWARDSTARTONLOGIN_H_
#define GAME_INTER_ONTIMEEVENTREWARDSTARTONLOGIN_H_

// df_game_r 还原 —— Inter_OnTimeEventRewardStartOnLogin（整点事件奖励登录启动分发器）。
// ORIG 符号：
//   _ZN35Inter_OnTimeEventRewardStartOnLogin12dispatch_sigEP5CUserPci    T 0x084e0f82
//   ~D1 0x084f708c / ~D0 0x084f70bc（编译器生成；vtable@0x8c8eb30
//   槽序 dispatch_sig / ~D1 / ~D0；D1 重置 vptr 后调
//   ISigDispatcher::~ISigDispatcher，见 0x84f708c）
#include "InterDispatcher.h"

class Inter_OnTimeEventRewardStartOnLogin : public ISigDispatcher
{
public:
    virtual int dispatch_sig(CUser* user, char* data, int len);
};

#endif  // GAME_INTER_ONTIMEEVENTREWARDSTARTONLOGIN_H_
