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
private:
    std::map<ENUM_SERVER_GROUP, stCirculationStatistic> m_data;
};

CCirculationStatistic* GetInstanceCirculationStatistic();

#endif
