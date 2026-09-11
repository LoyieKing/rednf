// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DropLayoutDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "DropLayoutDeps/getGoldGenTable.inc"


// ============================================================================
// CItemGenRateTable
// ============================================================================

#include "DropLayoutDeps/CItemGenRateTable.inc"


#include "DropLayoutDeps/dtor_CItemGenRateTable.inc"


#include "DropLayoutDeps/init.inc"


#include "DropLayoutDeps/destroy.inc"


#include "DropLayoutDeps/set_disable_drop_random_option_item.inc"


#include "DropLayoutDeps/_itemTypeVerify.inc"


#include "DropLayoutDeps/generateTable.inc"


// ============================================================================
// CSecialIMonsterDropItemGenRateTable
// ============================================================================

#include "DropLayoutDeps/CSecialIMonsterDropItemGenRateTable.inc"


#include "DropLayoutDeps/dtor_CSecialIMonsterDropItemGenRateTable.inc"


#include "DropLayoutDeps/init_1.inc"


#include "DropLayoutDeps/destroy_1.inc"


#include "DropLayoutDeps/_itemTypeVerify_1.inc"


#include "DropLayoutDeps/generateTable_1.inc"


// ============================================================================
// CMonsterDrop_Hell —— ctor W 0x0853a56a / D1 W 0x0853a60e / loadScript T 0x08535344
// ============================================================================

#include "DropLayoutDeps/CMonsterDrop_Hell.inc"


#include "DropLayoutDeps/dtor_CMonsterDrop_Hell.inc"


#include "DropLayoutDeps/loadScript.inc"


// ============================================================================
// CSpecialMonsterDrop —— ctor W 0x0853a426 / D1 W 0x0853a4ca / loadScript T 0x085373ee
// ============================================================================

#include "DropLayoutDeps/CSpecialMonsterDrop.inc"


#include "DropLayoutDeps/dtor_CSpecialMonsterDrop.inc"


#include "DropLayoutDeps/loadScript_1.inc"


// ============================================================================
// CMonsterDrop —— ctor W 0x0853a6c8 / loadScript T 0x08537938 / ×3 T 0x085380fa
// ============================================================================

#include "DropLayoutDeps/CMonsterDrop.inc"


#include "DropLayoutDeps/loadScript_2.inc"


#include "DropLayoutDeps/loadScript_3.inc"


// ============================================================================
// CObjectDrop —— ctor W 0x0853a7f8 / loadScript T 0x08538352
// ============================================================================

#include "DropLayoutDeps/CObjectDrop.inc"


#include "DropLayoutDeps/loadScript_4.inc"


// ============================================================================
// CClearReward —— ctor W 0x0853a968 / loadScript T 0x085392c0
// ============================================================================

#include "DropLayoutDeps/CClearReward.inc"


#include "DropLayoutDeps/loadScript_5.inc"


// ============================================================================
// CDungeonDrop —— ctor W 0x0853abc2（ORIG 无 loadScript 覆写符号）
// ============================================================================

#include "DropLayoutDeps/CDungeonDrop.inc"


// ============================================================================
// CGeneratorCommonData —— C1/D2 W 0x08374ebc/ec2（空体）/loadScript T 0x08534456 /
// getGoldGenTable T 0x08533424
// ============================================================================

#include "DropLayoutDeps/CGeneratorCommonData.inc"

#include "DropLayoutDeps/dtor_CGeneratorCommonData.inc"

#include "DropLayoutDeps/loadScript_6.inc"


}  // namespace WongWork

#include "DropLayoutDeps/WorldDropInfo.inc"
#include "DropLayoutDeps/dtor_WorldDropInfo.inc"

// ============================================================================
// random_option::CRandomOptionItemHandle::set_disable_drop —— W 0x08539e42
// （objdump：movb $0x1,0xc(%eax)。C1/D1 权威定义在 GlobalData.cpp。）
// ============================================================================
namespace random_option
{
#include "DropLayoutDeps/set_disable_drop.inc"

}  // namespace random_option

#include "DropLayoutDeps/get_gen_rate.inc"


#include "DropLayoutDeps/isSpecialMonsterDropItem.inc"


#include "DropLayoutDeps/isDropItemDungeonDifficulty.inc"


#include "DropLayoutDeps/isDropItemDungeonType.inc"


// ============================================================================
// CItemList::getItemGradeList —— W 0x8539e38（返回 +0x00 的等级分桶表指针）
// ============================================================================
__gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>, std::equal_to<int>,
                    std::allocator<CItem*> >* CItemList::getItemGradeList() const
{
    return m_gradeItemList;
}

#include "DropLayoutDeps/GetAutoMarketContitionsControl.inc"


// ============================================================================
// CAutoMarketConditionsControl::GetGoldDropAmountRate —— 报告 GetGoldDropAmountRate.md
// （ORIG T 0x082f89bc 逐分支转写；除数 -1000.0 为 .rodata 0x8c1fc70 双字实测）
// ============================================================================
__attribute__((target("arch=i486")))
#include "DropLayoutDeps/GetGoldDropAmountRate.inc"


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
