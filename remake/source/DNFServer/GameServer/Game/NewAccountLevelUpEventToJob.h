#ifndef GAME_NEWACCOUNTLEVELUPEVENTTOJOB_H_
#define GAME_NEWACCOUNTLEVELUPEVENTTOJOB_H_

// df_game_r 还原 —— NewAccountLevelUpEventToJob（新账号升级转职奖励 repeat
// 事件，event idx 0x68，经 CEventManager::GetRepeatEvent(0x68) 取得）。
// ORIG 派生自 CEventBase（vtable 0x08bbc948），无自身字段（sizeof 0x0c）。
// 全部符号按 docs/class_func_reports/NewAccountLevelUpEventToJob/*.md 转写：
//   C1 0x081b8170 / D1 0x081b818c / D0 0x081b81bc / StartEvent() 0x081b81da /
//   StartEvent(Word_Param) 0x081b81f6 / EndEvent() 0x081b824c /
//   StartAction() 0x081b8246 / IsEventing 0x081b8816（objdump 转写）/
//   processLevelUpToJob 0x081b82a4 / sendNewAccountLevelUpToJobData 0x081b829e
#include "CGuildServerProxy.h"   // CEventBase / Word_Param

class CUser;

class NewAccountLevelUpEventToJob : public CEventBase
{
public:
    NewAccountLevelUpEventToJob();                 // 0x081b8170
    virtual ~NewAccountLevelUpEventToJob();        // 0x081b818c / 0x081b81bc
    virtual void StartEvent();                     // 0x081b81da（vtable+0x10）
    virtual void StartEvent(Word_Param param);     // 0x081b81f6（vtable+0x18）
    virtual void EndEvent();                       // 0x081b824c（vtable+0x14）
    virtual void StartAction();                    // 0x081b8246（vtable+0x1c，空）
    virtual bool IsEventing(CUser* user) const;  // 0x081b8816（vtable+0x3c）

    bool processLevelUpToJob(CUser& user, bool growup);  // 0x081b82a4
    void sendNewAccountLevelUpToJobData(CUser& user);    // 0x081b829e（空）
};

#endif  // GAME_NEWACCOUNTLEVELUPEVENTTOJOB_H_
