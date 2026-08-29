#ifndef DNF_GAME_SIG_TYPES_H_
#define DNF_GAME_SIG_TYPES_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构类型（批量生成）
// 各结构成员布局待细化；当前用 char 数组占位以生成 GetInBuffer<T> 实例。
// ============================================================================
#include <cstddef>  // offsetof（SIG_* 布局 static_assert）

#include "Inven_Item.h"  // SIG_SAVE_INVENTORY 背包 Inven_Item 数组（pack(1)，0x3d）
#include "MissionTypes.h"  // MissionInfo（SIG_REQUEST_PVP_MISSION_LIST 任务列表）
#include "SigTypes_A.h"   // 分片 A（InformNoticeFlagData..SIG_CHECK_USER_SECU_REWARD）
#include "SigTypes_C.h"   // 分片 C（CHARAC_DATA/TAG_*/SIG_LOAD_* 域，SIG_LOGIN_DATA 之前）
#include "SigTypes_D.h"   // 分片 D（SIG_LOGIN_DATA 登录域 + MAILBOX..SAVE_EVENTITEM）
#include "SigTypes_E.h"   // 分片 E（SIG_SAVE_* 尾段 + 杂项/统计域）
#include "SigTypes_B.h"   // 分片 B（COMBO/CREATE/CREATURE/D..I 域，CHARAC_DATA 之前）
struct stAvatarEmblemInfo_t;  // GamePODTypes.h 完整定义（SIG_AVATAR_INSERT::setJewelSocket 引用）

#endif  // DNF_GAME_SIG_TYPES_H_
