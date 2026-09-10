// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DropLayoutDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DropLayoutDeps.cpp — WongWork 掉落生成器域：生命周期 + 脚本装载（独占 TU）
//
// 全部符号经 df_game_r nm 核实；实现逐条对照 docs/class_func_reports/
// 对应报告的 ## 汇编 + ## 反编译 C（无独立报告者以 objdump 直转并标注）。
// 类布局/声明唯一权威：CItemGenerator.h（本 TU 的姊妹批次扩写）。
//
// 本 TU 独家提供（勿在他处重复定义）：
//   WongWork::CGeneratorCommonData C1/D1/loadScript/getGoldGenTable
//   WongWork::{CMonsterDrop,CObjectDrop,CClearReward,CDungeonDrop} C1 与
//     loadScript（CDungeonDrop ORIG 无 loadScript 覆写符号，不实现）
//   WongWork::CMonsterDrop_Hell / CSpecialMonsterDrop C1/D1/loadScript
//   WongWork::CMonsterDrop::stFirstMobHuntingWeight_t C1 → 归 WongWorkDrop_A.cpp
//   WongWork::{CItemGenRateTable,CSecialIMonsterDropItemGenRateTable}
//     C1/~D1/init/destroy/generateTable/_itemTypeVerify/
//     set_disable_drop_random_option_item(仅前者)
//   WorldDropInfo C1/D1（objdump 0x08374ec8 / 0x08374ede 直转）
//   random_option::CRandomOptionItemHandle::set_disable_drop（W 0x08539e42；
//     C1/D1 复用 GlobalData.cpp 权威定义）
//   CItem::{get_gen_rate,isSpecialMonsterDropItem,isDropItemDungeonDifficulty,
//     isDropItemDungeonType}、CItemList::getItemGradeList
//   CDataManager::GetAutoMarketContitionsControl（W 0x084e92fc）
//   CAutoMarketConditionsControl::GetGoldDropAmountRate（T 0x082f89bc）
// ============================================================================

#include "GameTypes.h"       // cMyTrace（GetGoldDropAmountRate 千次日志路径）
#include <cstring>
#include <ctime>

#include "CItemGenerator.h"   // 域内全部类声明（含生成率表/Hell/Special）
#include "DNFLexWrapper.h"    // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include "CDataManager.h"     // G_CDataManager/m_itemList/CItem/CItemList 完整类型

namespace WongWork
{

// stFirstMobHuntingWeight_t（现 WongWork::CMonsterDrop 嵌套类型）C1 已迁移至
// WongWorkDrop_A.cpp（嵌套限定名；此处旧顶层定义删除以免 ODR/符号分裂）。

// ============================================================================
#include "DropLayoutDeps/getGoldGenTable.inc"
}

// ============================================================================
// CItemGenRateTable
// ============================================================================

#include "DropLayoutDeps/CItemGenRateTable.inc"
}

#include "DropLayoutDeps/dtor_CItemGenRateTable.inc"
}

#include "DropLayoutDeps/init.inc"
}

#include "DropLayoutDeps/destroy.inc"
}

#include "DropLayoutDeps/set_disable_drop_random_option_item.inc"
}

#include "DropLayoutDeps/_itemTypeVerify.inc"
}

#include "DropLayoutDeps/generateTable.inc"
}

// ============================================================================
// CSecialIMonsterDropItemGenRateTable
// ============================================================================

#include "DropLayoutDeps/CSecialIMonsterDropItemGenRateTable.inc"
}

#include "DropLayoutDeps/dtor_CSecialIMonsterDropItemGenRateTable.inc"
}

#include "DropLayoutDeps/init_1.inc"
}

#include "DropLayoutDeps/destroy_1.inc"
}

#include "DropLayoutDeps/_itemTypeVerify_1.inc"
}

#include "DropLayoutDeps/generateTable_1.inc"
}

// ============================================================================
// CMonsterDrop_Hell —— ctor W 0x0853a56a / D1 W 0x0853a60e / loadScript T 0x08535344
// ============================================================================

#include "DropLayoutDeps/CMonsterDrop_Hell.inc"
}

#include "DropLayoutDeps/dtor_CMonsterDrop_Hell.inc"
}

#include "DropLayoutDeps/loadScript.inc"
}

// ============================================================================
// CSpecialMonsterDrop —— ctor W 0x0853a426 / D1 W 0x0853a4ca / loadScript T 0x085373ee
// ============================================================================

#include "DropLayoutDeps/CSpecialMonsterDrop.inc"
}

#include "DropLayoutDeps/dtor_CSpecialMonsterDrop.inc"
}

#include "DropLayoutDeps/loadScript_1.inc"
}

// ============================================================================
// CMonsterDrop —— ctor W 0x0853a6c8 / loadScript T 0x08537938 / ×3 T 0x085380fa
// ============================================================================

#include "DropLayoutDeps/CMonsterDrop.inc"
}

#include "DropLayoutDeps/loadScript_2.inc"
}

#include "DropLayoutDeps/loadScript_3.inc"
}

// ============================================================================
// CObjectDrop —— ctor W 0x0853a7f8 / loadScript T 0x08538352
// ============================================================================

#include "DropLayoutDeps/CObjectDrop.inc"
}

#include "DropLayoutDeps/loadScript_4.inc"
}

// ============================================================================
// CClearReward —— ctor W 0x0853a968 / loadScript T 0x085392c0
// ============================================================================

#include "DropLayoutDeps/CClearReward.inc"
}

#include "DropLayoutDeps/loadScript_5.inc"
}

// ============================================================================
// CDungeonDrop —— ctor W 0x0853abc2（ORIG 无 loadScript 覆写符号）
// ============================================================================

#include "DropLayoutDeps/CDungeonDrop.inc"
}

// ============================================================================
// CGeneratorCommonData —— C1/D2 W 0x08374ebc/ec2（空体）/loadScript T 0x08534456 /
// getGoldGenTable T 0x08533424
// ============================================================================

#include "DropLayoutDeps/CGeneratorCommonData.inc"

#include "DropLayoutDeps/dtor_CGeneratorCommonData.inc"

#include "DropLayoutDeps/loadScript_6.inc"
}

}  // namespace WongWork

// ============================================================================
// WorldDropInfo 生命周期（本 TU 唯一 owner；无独立报告，objdump 直转）：
//   C1 0x08374ec8：仅构造 +0x04 的 vector<WorldDropItemChunck>；
//   D1 0x08374ede：仅析构该 vector；标量成员 m_first/m_total 无动作。
// ============================================================================
#include "DropLayoutDeps/WorldDropInfo.inc"
#include "DropLayoutDeps/dtor_WorldDropInfo.inc"
namespace random_option
{
#include "DropLayoutDeps/set_disable_drop.inc"
}
}  // namespace random_option

// ============================================================================
// CItem 访问器族（W inline；字段名见 CItem.h/STItemScript.h）
// ============================================================================
#include "DropLayoutDeps/get_gen_rate.inc"
}

#include "DropLayoutDeps/isSpecialMonsterDropItem.inc"
}

#include "DropLayoutDeps/isDropItemDungeonDifficulty.inc"
}

#include "DropLayoutDeps/isDropItemDungeonType.inc"
}

// ============================================================================
// CItemList::getItemGradeList —— W 0x8539e38（返回 +0x00 的等级分桶表指针）
// ============================================================================
__gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>, std::equal_to<int>,
                    std::allocator<CItem*> >* CItemList::getItemGradeList() const
{
    return m_gradeItemList;
}

// ============================================================================
// CDataManager::GetAutoMarketContitionsControl —— ORIG W 0x084e92fc
// （objdump：mov 0x8cc4(%eax),%eax —— 即成员 m_autoMarket）
// ============================================================================
#include "DropLayoutDeps/GetAutoMarketContitionsControl.inc"
}

// ============================================================================
// CAutoMarketConditionsControl::GetGoldDropAmountRate —— 报告 GetGoldDropAmountRate.md
// （ORIG T 0x082f89bc 逐分支转写；除数 -1000.0 为 .rodata 0x8c1fc70 双字实测）
// ============================================================================
__attribute__((target("arch=i486")))
#include "DropLayoutDeps/GetGoldDropAmountRate.inc"
}

// 布局自检（嵌入尺寸三重互证见文件头）
// ============================================================================
typedef char assert_drop_range[sizeof(WongWork::stGenItemRange_t) == 0x194 ? 1 : -1];
typedef char assert_rate_table[sizeof(WongWork::CItemGenRateTable) == 0x2356C ? 1 : -1];
typedef char assert_secial_table[sizeof(WongWork::CSecialIMonsterDropItemGenRateTable) ==
                                 0x211FCC ? 1 : -1];
typedef char assert_firstmob[sizeof(WongWork::stFirstMobHuntingWeight_t) == 8 ? 1 : -1];
typedef char assert_opt_handle[sizeof(random_option::CRandomOptionItemHandle) == 0x18 ? 1 : -1];
typedef char assert_hell[sizeof(WongWork::CMonsterDrop_Hell) == 0x237C0 ? 1 : -1];
typedef char assert_special[sizeof(WongWork::CSpecialMonsterDrop) == 0x21230C ? 1 : -1];
typedef char assert_monster[sizeof(WongWork::CMonsterDrop) == 0x2593a4 ? 1 : -1];
typedef char assert_object[sizeof(WongWork::CObjectDrop) == 0x2593a4 ? 1 : -1];
typedef char assert_clear[sizeof(WongWork::CClearReward) == 0x23b84 ? 1 : -1];
typedef char assert_dungeon[sizeof(WongWork::CDungeonDrop) == 0x18 ? 1 : -1];
