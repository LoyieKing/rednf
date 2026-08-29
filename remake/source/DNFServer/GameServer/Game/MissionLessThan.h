#ifndef GAME_MISSIONLESSTHAN_H_
#define GAME_MISSIONLESSTHAN_H_

class CMission;

class MissionLessThan
{
public:
    // ORIG T 0x85e709e _ZN15MissionLessThanclEPK8CMissionS2_：
    // 先比 GetMissionKind 再比 GetMissionIndex；setl %al 直返 → bool。
    bool operator()(CMission const*, CMission const*);
};

#endif  // GAME_MISSIONLESSTHAN_H_
