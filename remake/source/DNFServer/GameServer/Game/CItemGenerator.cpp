#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemGenerator/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CItemGenerator.cpp — WongWork 掉落生成器域权威实现（唯一 TU）
//
// 提供以下 ORIG 符号（全部经 df_game_r nm/objdump 核实）：
//   _ZN8WongWork10CLuckPointC1Ev / D1Ev            0x08550884 / 0x085508ec
//   _ZN8WongWork19stGenerateRefData_tC1Ev / D1Ev   0x0814a706 / 0x0814aaf0
//   _ZN8WongWork18stGenerateResult_tC1Ev / D1Ev    0x0814ab06 / 0x0814ab4c
//   _ZN8WongWork21CGenerateRandomNumberC1Ei / D1Ev 0x085334ea / 0x085335d0
//   _ZN8WongWork21CGenerateRandomNumber14generateNumberEii 0x085334a4
//   _ZN8WongWork21GenerateRandomUpgradeC1Ev / D1Ev 0x085335ee / 0x0853364e
#include "CItemGenerator.h"

#include "CDataManager.h"
#include "ServerParameterScript.h"
#include "CUser.h"
#include "CItemUpgrade_Separate.h"   // GenuineDamageUpgradeTable 完整类型 +
                                     //   importGenuineUpgradeTable 声明

#include "GameRand.h"   // get_rand_int（ORIG 0x086b1b87）

#include <new>          // placement new / operator new（getGenuineUpgradeTable）
#include <string.h>     // memset
#include <time.h>       // time(0)（CGenerateRandomNumber/GenerateRandomUpgrade 播种）

namespace WongWork
{

// [b28d-Main·检查点18] 原 `WongWork::CLuckPoint` ctor/dtor 定义已删（ORIG 无该符号；
//   该名字现为 `using ::CLuckPoint`，其 ctor/dtor 由 CClearReward.cpp 定义）。

#include "CItemGenerator/stGenerateRefData_t.inc"


#include "CItemGenerator/dtor_stGenerateRefData_t.inc"


#include "CItemGenerator/stGenerateResult_t.inc"
#include "CItemGenerator/dtor_stGenerateResult_t.inc"

#include "CItemGenerator/CGenerateRandomNumber.inc"


#include "CItemGenerator/dtor_CGenerateRandomNumber.inc"


#include "CItemGenerator/generateNumber.inc"


#include "CItemGenerator/GenerateRandomUpgrade.inc"


#include "CItemGenerator/dtor_GenerateRandomUpgrade.inc"


#include "CItemGenerator/CItemGenerator.inc"


#include "CItemGenerator/dtor_CItemGenerator.inc"


#include "CItemGenerator/setDropRatio.inc"


#include "CItemGenerator/loadScript.inc"


#include "CItemGenerator/generateRandomItem.inc"


#include "CItemGenerator/generateSpecificItem.inc"


#include "CItemGenerator/setCommonData.inc"


#include "CItemGenerator/getGeneratorCommonData.inc"


#include "CItemGenerator/setGeneratorType.inc"


#include "CItemGenerator/getGeneratorType.inc"


}  // namespace WongWork

// ============================================================================
// ::getGenuineUpgradeTable —— ORIG T 0x0891229d 逐指令转写：
//   函数局部 static 指针（bss 符号 _ZZ22getGenuineUpgradeTablevE
//   25genuineDamageUpgradeTable 与 ORIG 同名）；首次为 0 时：
//     operator new(0x188) → 清零（ORIG gcc4.1 的 value-init 内联 rep stos；
//     gcc4.4 对 new T() 不再生成清零，故显式 memset 保持语义）→
//     placement 构造 GenuineDamageUpgradeTable（仅三 map）→
//     importGenuineUpgradeTable("Etc/Upgrade_Separate.etc", t) 失败返回 0
//     （static 保留，与 ORIG 一致：失败仅本次返回 0）。
// ============================================================================
// [本波·N77/N78 封顶] ORIG 0x0891229d（51 insns）的 value-init 清零是 4.1
//   内联 `rep stos`（5 条：mov $0/%eax、mov $0x62/%edx、%/edi、%/ecx、rep stos）；
//   4.4 对 `new T()` 依 CWG434 不发射清零、对 memset 恒发 call（探针族 /tmp 全败）
//   ⇒ 结构不可复现。本波把实现压到最接近且语义忠实的形态：
//     operator new → memset(0x188) → placement 构造 → import
//   （原先额外一层 `void* mem` 具名局部 + placement-new 的 4.4 EH 段已删）。
//   实测该件 51/52 条：残差 = rep stos 5 条 vs memset 4 条（3 准备 + call）+ 4.4
//   对 placement-new 的 landing 段；语义逐位等价（同一清零区间 + 同一 ctor）。
GenuineDamageUpgradeTable* getGenuineUpgradeTable()
{
    static GenuineDamageUpgradeTable* genuineDamageUpgradeTable = 0;
    if (genuineDamageUpgradeTable == 0)
    {
        genuineDamageUpgradeTable =
            (GenuineDamageUpgradeTable*)operator new(0x188);
        memset(genuineDamageUpgradeTable, 0, 0x188);
        new (genuineDamageUpgradeTable) GenuineDamageUpgradeTable();
        if (!importGenuineUpgradeTable("Etc/Upgrade_Separate.etc",
                                       genuineDamageUpgradeTable))
            return 0;
    }
    return genuineDamageUpgradeTable;
}

#include "CItemGenerator/UseLuckPoint.inc"

