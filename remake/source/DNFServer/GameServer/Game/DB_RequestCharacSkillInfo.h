#ifndef GAME_DB_REQUESTCHARACSKILLINFO_H_
#define GAME_DB_REQUESTCHARACSKILLINFO_H_

// ============================================================================
// df_game_r 还原 —— DB_RequestCharacSkillInfo（DB 线程角色技能信息请求分发器）
// 报告：docs/class_func_reports/DB_RequestCharacSkillInfo/（4 符号）：
//   C1/C2 T @0x0844fcaa（仅设 vptr，sz 0x4；定义在 W8LT_DBTask.cpp 内联提供）/
//   dispatch T @0x084474f2 / sendResult T @0x084475a0 /
//   _GetSkillTreeIndex T @0x08447162 / _GetSkillInfo T @0x08447312。
// 布局：仅基类 DBDispatcher 子对象，sizeof = 4（vptr）。
// 实现唯一 owner：DB_RequestCharacSkillInfo.cpp。
// ============================================================================

#include "DBDispatcher.h"

class Stream;
struct SIG_REQUEST_CHARAC_SKILL_INFO;
struct _Mastered_skill;

class DB_RequestCharacSkillInfo : public DBDispatcher
{
public:
    DB_RequestCharacSkillInfo();                       // C1/C2 T @0x0844fcaa（W8LT_DBTask.cpp）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // T @0x084474f2

    bool _GetSkillTreeIndex(unsigned int characNo, char* outIndex);  // T @0x08447162
    bool _GetSkillInfo(unsigned int characNo, char slot, _Mastered_skill* skills);  // T @0x08447312
    void sendResult(int userIdx, SIG_REQUEST_CHARAC_SKILL_INFO* sig);  // T @0x084475a0
};

#endif  // GAME_DB_REQUESTCHARACSKILLINFO_H_
