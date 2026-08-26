#ifndef GAME_SCHOOLMATCHENUMS_H_
#define GAME_SCHOOLMATCHENUMS_H_

// ============================================================================
// 校际赛 / ELO 评级枚举（ORIG mangled 引用，df_game_r）
// 数值证据：_GetOtherTeam(0→1,1→0,else 2)、_GetOppositeTeam(1↔2) 等分支，
// 枚举项命名与零值为推断命名（ORIG 调试信息未提供成员名）。
// ============================================================================

enum ENUM_SCHOOL_MATCH_TEAM
{
    SCHOOL_MATCH_TEAM_0 = 0,
    SCHOOL_MATCH_TEAM_1 = 1,
    SCHOOL_MATCH_TEAM_2 = 2
};

#ifndef DNF_ENUM_ELO_RATING_SYSTEM_DEFINED
#define DNF_ENUM_ELO_RATING_SYSTEM_DEFINED
enum ENUM_ELO_RATING_SYSTEM
{
    ENUM_ELO_RATING_0 = 0
};
#endif

#endif  // GAME_SCHOOLMATCHENUMS_H_
