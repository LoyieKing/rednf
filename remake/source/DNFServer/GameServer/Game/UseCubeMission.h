// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/UseCubeMission/
// UseCubeMission：ORIG ctor T @0x08165920、dtor D0 @0x08165974 / D1 @0x08165944、
// processMission T @0x08165992、rewardMission T @0x08165a2e。
// 成员/布局无报告证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_USECUBEMISSION_H_
#define DNF_GAME_USECUBEMISSION_H_

#include "HeroMissionCondition.h"

class CUser;
class HeroMissionValue;

class UseCubeMission : public BaseHeroMissionCondition
{
public:
    UseCubeMission();
    virtual ~UseCubeMission();
    virtual void processMission(CUser* user, HeroMissionValue& value, unsigned int idx);
    virtual bool rewardMission(CUser* user, HeroMissionValue& value);
};

#endif  // DNF_GAME_USECUBEMISSION_H_
