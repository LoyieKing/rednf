// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Village/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r Village（G2-5 村庄对象，sizeof 0x34）
// 逐函数对照 docs/class_func_reports/Village.md 与 ORIG 反汇编还原。
// 布局（ORIG ctor 086c32e0 / set_village 086c33b8 推导）：
//   +0x00 int    m_villageId      （set_village 写入 town script +0）
//   +0x04 pad（4）
//   +0x08 map<ushort,CUser*> m_users（0x18 字节，ctor 调 _ZNSt3map...C1Ev）
//   +0x20 bool   m_bEnabled
//   +0x24 int    m_areaCount
//   +0x28 Area*  m_pAreas         （new[] 数组，前 4 字节存元素个数）
//   +0x2c int    m_field2c        （set_village 写入 town script +0x3c）
//   +0x30 int    m_questIdx        （set_village 写入 town script +0x40；CheckMoveTown 读作 quest idx）
// 跨类调用一律 include 真实头（CDataManager.h / GameWorld.h / GameTypes.h /
// PacketGuard.h / TownAreaScript.h），std::map/vector 使用真实容器，
// 不再使用 extern asm 桥。
// ============================================================================

#include "GameTypes.h"        // cMyTrace
#include "CDataManager.h"     // G_CDataManager / CDataManager::get_dungeon
#include "CDungeon.h"         // CDungeon::m_vec6d4（set_village +0x6d4 具名化）
#include "PacketGuard.h"      // PacketGuard
#include "TownAreaScript.h"   // TownAreaScript
#include "CSchoolMgr.h"       // CSchoolMgr::SetSchoolArea / g_schoolMgr
#include "CGuildAgitManager.h" // CGuildAgitManager::SetGuildAgitAreaDomain / g_guildAgitMgr
#include "GameWorld.h"        // GameWorld::IsSchoolPvPChannel / G_GameWorld
#include "Village.h"          // Village（本 TU 实现）
#include "Area.h"             // Area（0x98 权威头）
#include "STQuestScript.h"    // TownScript（0x44 权威头）
#include "CMap.h"             // STMapScript（0x37c 权威头）
#include "CSchoolMgr.h"       // CBelong 完整类型（本 TU 承载其 C1 定义）

#include <map>
#include <new>
#include <string>
#include <vector>

// ImportMapScript（ORIG 0x089dcf54，自由函数；定义于 STMapScript.cpp，无权威头）
#include "Village/ImportMapScript.inc"

#include "Village/Village.inc"
}

#include "Village/dtor_Village.inc"
}

// [b28d-62] N18b：cmp 操作数序（写 `m_pAreas == pEnd` 亦不可翻）+ N112 对齐
//   nop；39=39，p167 CLEAR。ORIG 0x086c3348
#include "Village/destroy.inc"
}

// [b28d-62] N89/P50：ORIG 用 `Area::~Area` + `operator delete[]`（数组删除），
//   我方 placement new/delete 形态 + 迭代器 cv 差。ORIG 0x086c33b8
#include "Village/set_village.inc"
}

#include "Village/getArea.inc"
}

#include "Village/get_user_id_list.inc"
}

#include "Village/send_to_area.inc"
}

#include "Village/GetUserCount.inc"
}

// CBelong::CBelong 定义置于本 TU（与消费调用者分离）：4.4 unit-at-a-time
// 对 TU 内可见 callee 塌缩 outgoing 帧（CSchoolMemberC1 sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 p6/p7 实证同族）。
#include "Village/CBelong.inc"
}
