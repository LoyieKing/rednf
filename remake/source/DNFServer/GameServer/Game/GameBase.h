#ifndef GAME_GAMEBASE_H_
#define GAME_GAMEBASE_H_

// df_game_r Game/ 全局基础类型统一入口（2026-08-16 集成）。
// Mutex / Guard / DynamicPool / cMyTrace 唯一权威定义见 GameTypes.h。
#include "GameTypes.h"
#include "Stream.h"
#include "CStreamGuard.h"

long long SECOND_TO_MILISECOND(int sec);

#endif  // GAME_GAMEBASE_H_
