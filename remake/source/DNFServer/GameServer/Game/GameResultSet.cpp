// ============================================================================
// df_game_r 还原 —— GameResultSet。
// ORIG：ctor 0x84b75d2 T / dtor 0x84b75e6 T / Clear 0x84b75fa T，
// 三者均为对 GameResultType 同名方法的转发调用。
// 报告：docs/class_func_reports/GameResultSet/{~GameResultSet,GameResultSet,Clear}.md
// 类声明唯一来源：GameResultType.h / GameResultSet.h 权威头。
// ============================================================================
#include "GameResultSet.h"

// ORIG ctor 0x84b75d2 T：转发基类构造（0x84b73d4）。
GameResultSet::GameResultSet()
{
}

// ORIG dtor 0x84b75e6 T：转发基类非内联析构（0x84b7416）。
GameResultSet::~GameResultSet()
{
}

// ORIG 0x84b75fa T：显式限定转发 GameResultType::Clear（0x84b7442）。
void GameResultSet::Clear()
{
    GameResultType::Clear();
}
