// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/PvpResultSet/
// PvpResultSet：PvpResultType 派生薄封装（ctor/dtor/Clear 仅转发基类）。
// 定义源：Game/PvpResultSet.cpp:15（本批复制为头声明，实现仍在该 cpp）。
#ifndef DNF_GAME_PVPRESULTSET_H_
#define DNF_GAME_PVPRESULTSET_H_

#include "PvpResultType.h"

class PvpResultSet : public PvpResultType
{
public:
    PvpResultSet();        // ORIG T @0x085d5f42
    ~PvpResultSet();       // ORIG T @0x085d5f56
    void Clear();          // ORIG T @0x085d5f6a
};

#endif  // DNF_GAME_PVPRESULTSET_H_
