// df_game_r Game/ CDailyScheduleManager（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CDAILYSCHEDULEMANAGER_H
#define DNF_CDAILYSCHEDULEMANAGER_H

#include <vector>
#include "GameEnums.h"

class CDailyScheduleManager
{
public:
    void AddDailSchedule(const std::vector<int>& schedule);
    int GetScheduleState(_DailyScheduleType type, unsigned int idx) const;
};

#endif  // DNF_CDAILYSCHEDULEMANAGER_H