#ifndef GAME_LEVELUPBEFORE70LVEVENT_H_
#define GAME_LEVELUPBEFORE70LVEVENT_H_

// df_game_r 还原 —— LevelUpBefore70LvEvent（70 级前升级 repeat 事件实体）。
// ORIG 派生自 CEventBase（vtable 0x08b59048），无自身字段（sizeof 0x0c）。
// 全部 6 个符号按 docs/class_func_reports/LevelUpBefore70LvEvent/*.md 转写：
//   C1 0x0811ff60 / D1 0x0811ff7c / D0 0x0811ffac / StartEvent() 0x0811ffca /
//   StartEvent(Word_Param) 0x0812001c / EndEvent() 0x0812006e
// （StartEvent()/EndEvent()/StartEvent(Word_Param) 为虚覆盖，日志用
//   __FUNCTION__ —— ORIG .rodata 符号 _ZZN22...E12__FUNCTION__ 实证）。
#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class LevelUpBefore70LvEvent : public CEventBase
{
public:
    LevelUpBefore70LvEvent();                      // 0x0811ff60
    virtual ~LevelUpBefore70LvEvent();             // 0x0811ff7c / 0x0811ffac
    virtual void StartEvent();                     // 0x0811ffca（vtable+0x10）
    virtual void StartEvent(Word_Param param);     // 0x0812001c（vtable+0x18）
    virtual void EndEvent();                       // 0x0812006e（vtable+0x14）
    virtual bool IsEventing(CUser* user) const;    // 0x0812011c（恒返 1）
};

#endif  // GAME_LEVELUPBEFORE70LVEVENT_H_
