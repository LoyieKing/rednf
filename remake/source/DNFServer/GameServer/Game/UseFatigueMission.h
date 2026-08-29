#ifndef DNF_GAME_USEFATIGUEMISSION_H_
#define DNF_GAME_USEFATIGUEMISSION_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/UseFatigueMission/
// ORIG T 符号：C1 0x08165694 / D1 0x081656b8 / D0 0x081656e8 /
// processMission 0x08165706 / rewardMission 0x081657a2。

#include "HeroMissionCondition.h"   // BaseHeroMissionCondition / HeroMissionValue

class CUser;

// 英雄任务条件类：ctor 调 BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T=2)
// 后写 vptr（vtable 实证，类为多态）；dtor 有 D0，故析构虚。
class UseFatigueMission : public BaseHeroMissionCondition
{
public:
    UseFatigueMission();                                // T 0x08165694
    virtual ~UseFatigueMission();                       // D1 0x081656b8 / D0 0x081656e8
    // 消耗疲劳推进任务值：value+0x04 ushort 与 vector<unsigned short>[0]
    //（this+0x28 上限）比较/钳制。
    virtual void processMission(CUser* user, HeroMissionValue& value, unsigned int amount);   // T 0x08165706
    virtual bool rewardMission(CUser* user, HeroMissionValue& value);                          // T 0x081657a2
};
// 注：processMission/rewardMission 的 virtual 属性为【推断】（类有 vtable 但
// ORIG vtable 槽位布局未见报告；返回类型按反编译确认均为 void）。

#endif  // DNF_GAME_USEFATIGUEMISSION_H_
