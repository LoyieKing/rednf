// df_game_r 还原 —— EventNewCharacterReward（新手角色活动奖励，事件实体）。
// ORIG 派生自 CEventBase（vtable 0x08b80028）：0/1 虚析构，2/3 =
// StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，
// 含参数校验与日志；ORIG 0x0816bb94 汇编实证虚调 vptr+0x08 → StartEvent()），
// 其余槽继承 CEventBase。自身字段：
//   +0x0c m_job（活动角色职业，ctor 置 -1）、+0x10 m_field10（经验千分数，
//   ctor 置 0；StartEvent(Word_Param) 以 param 两个 ushort 写入并同步
//   m_addInfo）。全部 9 个符号按
//   docs/class_func_reports/EventNewCharacterReward/*.md 转写。
#ifndef GAME_EVENTNEWCHARACTERREWARD_H_
#define GAME_EVENTNEWCHARACTERREWARD_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param

class EventNewCharacterReward : public CEventBase
{
public:
    EventNewCharacterReward();                     // 0x0816bac4
    virtual ~EventNewCharacterReward();            // 0x0816baf4 / 0x0816bb24
    virtual void StartEvent();                     // 0x0816bb42（vtable 槽 2）
    virtual void StartEvent(Word_Param param);     // 0x0816bb94（vtable 槽 4）
    virtual void EndEvent();                       // 0x0816bc98（vtable 槽 3）

    // 活动角色职业（+0x0c）：param 是否活动角色。
    bool isEventCharacter(int job);                // 0x0816bcea
    // 事件角色经验比例（+0x10 千分数 / 100.0f）。
    float eventExpRatio();                         // 0x0816bd06
    // 活动角色职业直读（+0x0c）。
    int eventCharacterJob();                       // 0x0816bd22

private:
    int m_job;      // +0x0c（ctor 置 0xffffffff；StartEvent(Word_Param) 写低 ushort）
    int m_field10;  // +0x10（ctor 置 0；StartEvent(Word_Param) 写高 ushort）
};

#endif  // GAME_EVENTNEWCHARACTERREWARD_H_
