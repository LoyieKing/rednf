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
    // [M4-S1 直证] ORIG 0x0860de46 在 for 退出路径 `mov -0x4(%ebp),%ebx; leave; ret` 之前有
    //   `mov $0x1,%eax`（Ghidra 作 undefined4）⇒ 返回类型 bool；不入 mangled，调用点全部忽略
    //   返回值（DispLinkA2.cpp:1468 / TimerDispatcher.cpp:3889）⇒ 零 ABI 影响。
    bool InsertCirculationStatistic();
private:
    std::map<ENUM_SERVER_GROUP, stCirculationStatistic> m_data;
};

CCirculationStatistic* GetInstanceCirculationStatistic();

#endif
