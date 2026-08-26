// ============================================================================
// CCreature.h —— user_creature::CCreature 域实现 TU 的同名入口头。
//
// 权威类声明宿主：CGameManager.h（user_creature 命名空间）
//   - user_creature::CCreature   （镜像尺寸 0x100，字段已具名化）
//   - user_creature::CStomach    （8B，内嵌于 CCreature +0x34）
//   - user_creature::COverSkillTime（4B，内嵌于 CCreature +0x50）
// 本头不重复定义任何类型（避免 ODR），仅统一 include 入口；
// 全部成员函数的真实实现在同目录 CCreature.cpp，逐函数依据
// docs/class_func_reports/user_creature__CCreature/、user_creature__CStomach/、
// user_creature__COverSkillTime/ 的 ##汇编 + ##反编译C 转写。
// ============================================================================
#ifndef GAME_CCreature_h
#define GAME_CCreature_h

#include "CGameManager.h"

#endif  // GAME_CCreature_h
