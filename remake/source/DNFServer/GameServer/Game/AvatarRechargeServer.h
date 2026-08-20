#ifndef DNF_GAME_AVATARRECHARGESERVER_H_
#define DNF_GAME_AVATARRECHARGESERVER_H_

// df_game_r Game/ AvatarRechargeServer（独立 TU，从 GameStubs.cpp 拆出）。
// 布局：pad(+0x00, 0x38B) / m_emblemGradeRateMap(+0x38)。
#include <map>

namespace arad { struct SigAradAvatarEnduranceSave; }

class AvatarRechargeServer
{
public:
    static int GetRateFromEmblemGrade(int grade);
    static void SaveAvatarEnduranceUnit(int key, int value);
    static void SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save);

    char m_pad[0x38];
    std::map<int, int> m_emblemGradeRateMap;  // +0x38
};

#endif  // DNF_GAME_AVATARRECHARGESERVER_H_
