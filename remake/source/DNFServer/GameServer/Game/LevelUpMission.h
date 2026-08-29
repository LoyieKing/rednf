#ifndef GAME_LEVELUPMISSION_H_
#define GAME_LEVELUPMISSION_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "HeroMissionCondition.h"

class CUser;
class HeroMissionValue;

class LevelUpMission : public BaseHeroMissionCondition
{
public:
    LevelUpMission();
    virtual ~LevelUpMission();
    virtual bool rewardMission(CUser*, HeroMissionValue&);
    virtual void processMission(CUser*, HeroMissionValue&, unsigned int);
};

#endif  // GAME_LEVELUPMISSION_H_
