#ifndef DNF_GAME_STREAMDEPS_H_
#define DNF_GAME_STREAMDEPS_H_

// Stream 家族依赖统一入口（2026-08-16 集成）。
// Mutex / Guard / DynamicPool / LogManager 见 GameTypes.h；
// CEnvironment 完整定义见 CEnvironment.h。
#include "GameTypes.h"

class CEnvironment;
extern CEnvironment* G_CEnvironment();

#endif  // DNF_GAME_STREAMDEPS_H_
