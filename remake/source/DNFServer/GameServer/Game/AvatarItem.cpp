// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AvatarItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::Avatar_Item 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。
#include "AvatarItem.h"
#include <cstddef>
#include <cstring>

static_assert(sizeof(stAvatarEmblemInfo_t) == 0x1e, "avatar emblem ABI layout changed");
static_assert(offsetof(stAvatarEmblemInfo_t, stEmblemSocket) == 0, "avatar emblem socket offset changed");
static_assert(sizeof(WongWork::Avatar_Item) == 0x44, "Avatar_Item ABI size changed (ORIG packed 0x44, DynamicPool MoreAlloc 步长 0x4c)");
static_assert(offsetof(WongWork::Avatar_Item, m_emblemInfo) == 0x21, "Avatar_Item emblem offset changed");
static_assert(offsetof(WongWork::Avatar_Item, m_color1) == 0x3f, "Avatar_Item color1 offset changed (ORIG 0x3f)");
static_assert(offsetof(WongWork::Avatar_Item, m_color2) == 0x41, "Avatar_Item color2 offset changed (ORIG 0x41)");

namespace WongWork
{

// ORIG _ZN8WongWork11Avatar_Item4initEv W 0x82fa35a：仅清 +0x00/+0x04 两个 int、
// memset(m_ipgAgencyNo,0,0x19)、memset(+0x3f,0,4)；emblem 区与 +0x43 之后不清。
#include "AvatarItem/init.inc"
}

#include "AvatarItem/GetExpireDate.inc"

#include "AvatarItem/GetRegistTime.inc"

#include "AvatarItem/GetRemainDate.inc"
}

#include "AvatarItem/GetIPGAgencyNo.inc"

#include "AvatarItem/getJewelSocketData.inc"
}

#include "AvatarItem/getExpansionInfo.inc"

#include "AvatarItem/getExnansionInfoRef.inc"

#include "AvatarItem/setExpansionInfo.inc"
}

#include "AvatarItem/SetData.inc"
}

#include "AvatarItem/setEmblemSocketData.inc"
}

// [b28d-Main·检查点18] 原 `updateEmblemSocket(int, unsigned long)` 定义已删（发明签名）；
//   权威定义在 WongWorkMiscB_A.cpp `updateEmblemSocket(unsigned int, unsigned int)` 0x082fa2a6。

}  // namespace WongWork
