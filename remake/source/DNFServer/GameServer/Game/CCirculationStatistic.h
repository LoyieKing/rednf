#ifndef GAME_CCIRCULATIONSTATISTIC_H_
#define GAME_CCIRCULATIONSTATISTIC_H_

#include <map>
#include "GameTypes.h"
#include "CEnvironment.h"

class CUser;
enum CIRCULATION_STATISTIC_FIELD { FIELD_0 = 0 };
class CCirculationStatistic
{
public:
    struct stCirculationStatistic
    {
        int m_values[0x47][0x30];
        void reset();
    };
    bool AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user, unsigned int value);
    // ORIG T 0x0860dccc（DB_InsertCirculationStatistic 生产；Timer_TenMinuteSchedule p3==1 分支）
    void InsertCirculationStatistic();
private:
    std::map<ENUM_SERVER_GROUP, stCirculationStatistic> m_data;
};

CCirculationStatistic* GetInstanceCirculationStatistic();

#endif
