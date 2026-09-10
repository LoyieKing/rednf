// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTraceMobDieHack/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CTraceMobDieHack 还原（G2-5 批次）。
// CParty +0x210，总尺寸 0x94。符号（nm -C，W/T 全实现）：
//   C1 0x082a517c / D1 0x082a5236 / reset 0x082a50b0 /
//   traceBegin 0x085bf4cc / traceEnd 0x08599a88 /
//   addMonsterDamage 0x085bf4f6 / setMobKiller 0x085bf628 /
//   setDungeonDiff 0x085bf646 / reportHackInfo 0x08599cb2 /
//   _collect 0x08599f64；附带 stMonsterDamageLog_t C1 0x085bf4a2、
//   stHackLog_t::operator== 0x085bf472。
// 布局（由 ctor/reset/traceEnd/reportHackInfo 反汇编推导）：
//   +0x00 long m_traceBeginTime、+0x04 map<ushort,stMonsterDamageLog_t>[2]
//   （各 0x18）、+0x34 vector<int>（0xc）、+0x40 vector<stHackLog_t>（0xc）、
//   +0x4c char m_bTracing、+0x50 CUser* m_users[4]、+0x54 int m_damage[4]、
//   +0x58 int m_count[4]、+0x80 bool m_bInspectUser、+0x84 int m_dungeonDiff、
//   +0x88 保留。类/结构定义见 CTraceMobDieHack.h（真实 std::map/vector 成员，
// b28d-45 判词汇总（本 TU 1 件）：
//   traceEnd 0x08599a88 DIFF107 N82/N98：182:182 槽位集对齐；余 getCurSec
//     重定位显示（R_386_32）+ 帧槽差。
//   ctor/dtor 由编译器按成员序隐式构造/逆序析构，与 ORIG 一致）。
// 跨类调用一律 include 真实头，不再使用 extern asm 桥。

#include <algorithm>

#include "CTraceMobDieHack.h"
#include "CUser.h"              // CUser / CUserCharacInfo（基类）/ WongWork::CHackAnalyzer
#include "CDungeon.h"           // CDungeon::get_index / get_standard_level
#include "CParty.h"             // CParty::m_dungeon（+0xcac）
#include "CSystemTime.h"        // CSystemTime::getCurSec
#include "CVillageMonsterMgr.h" // village_attacked::CVillageMonsterMgr::OnEvent
#include "GlobalData.h"         // GlobalData::s_systemTime_ / s_villageMonsterMgr

// CParty::m_dungeon（+0xcac）在 CParty.h 为 public 数据成员，直接具名访问。

#include "CTraceMobDieHack/stMonsterDamageLog_t.inc"
}

bool stHackLog_t::operator==(const stHackLog_t& other) const
{
    return m_accId == other.m_accId && m_characNo == other.m_characNo;
}

#include "CTraceMobDieHack/CTraceMobDieHack.inc"
}

#include "CTraceMobDieHack/dtor_CTraceMobDieHack.inc"
}

#include "CTraceMobDieHack/reset.inc"
}

#include "CTraceMobDieHack/traceBegin.inc"
}

#include "CTraceMobDieHack/traceEnd.inc"
}

#include "CTraceMobDieHack/addMonsterDamage.inc"
}

#include "CTraceMobDieHack/setMobKiller.inc"
}

#include "CTraceMobDieHack/setDungeonDiff.inc"
}

#include "CTraceMobDieHack/reportHackInfo.inc"
}

#include "CTraceMobDieHack/_collect.inc"
}
