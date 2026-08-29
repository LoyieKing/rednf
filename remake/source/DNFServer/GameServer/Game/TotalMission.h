// TotalMission —— Hero Mission「合计任务」条件类（MissionNo T_3 分支）。
// ORIG 实证：ctor 0x08166306 调 BaseHeroMissionCondition C1 后写 vptr 0x8b7f048
// （objdump 0x8166306..0x8166329），故继承 BaseHeroMissionCondition；
// dtor D1 0x0816632a / D0 0x0816635a。
// UserMiscLink 批次改权威声明（原 HdrCreate3 独立类声明无使用者，已替换）。
#ifndef DNF_GAME_TOTALMISSION_H_
#define DNF_GAME_TOTALMISSION_H_

#include <vector>

#include "HeroMissionCondition.h"   // BaseHeroMissionCondition / MissionNo / 布局基

class CUser;
class HeroMissionValue;

class TotalMission : public BaseHeroMissionCondition
{
public:
    TotalMission();   // ORIG T @0x08166306
    ~TotalMission();  // ORIG D1 @0x0816632a / D0 @0x0816635a

    void processMission(CUser*, HeroMissionValue&, unsigned int);  // ORIG T @0x08166378（返回类型【推断】）
    // ORIG T @0x081663f4：函数体恒返回 1（报告 TotalMission/rewardMission.md 全文转写）。
    bool rewardMission(CUser*, HeroMissionValue&);
    // ORIG T @0x081663fe（报告 rewardMission_081663fe.md；HeroMissionEvent::rewardMission
    // missionType==3 分支直调）。定义 Game/UserMiscLink.cpp。
    bool rewardMission(CUser*, HeroMissionValue&,
                       std::vector<HeroMissionValue>&);
};

#endif  // DNF_GAME_TOTALMISSION_H_
