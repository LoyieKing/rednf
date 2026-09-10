// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_Verify/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Arad_DataManager.h"    // ARAD::Arad_InterfaceGameScript 完整类型（虚槽2/3）
#include "ARAD_FN_DeleteGameScript.h"
#include "CItem.h"
#include "Inven_Item.h"

#include <utility>

// ORIG mangled 内嵌枚举：N9ARAD_TYPE14INVENTORY_TYPEE = ARAD_TYPE::INVENTORY_TYPE。
// 值域 0..3 与 ORIG 全局 INVENTORY_TYPE（LinkClosureD.cpp 建模）一致【推断】；
// arad_equipment_verify 不读取该参数，值不影响函数行为。
namespace ARAD_TYPE
{
    enum INVENTORY_TYPE
    {
        INVENTORY_TYPE_0 = 0,
        INVENTORY_TYPE_1 = 1,
        INVENTORY_TYPE_2 = 2,
        INVENTORY_TYPE_3 = 3
    };
}

namespace ARAD
{

// ORIG T 0x0819eaee（10 字节）：mov $0x0,%eax; ret。
#include "Arad_Verify/arad_equipment_verify.inc"

// ORIG T 0x0819eae4（10 字节）：mov $0xb0,%eax; ret。
#include "Arad_Verify/arad_stackable_verify.inc"

// ORIG W 0x08188c56：destroyAradData 的 for_each 删除函子。
// 汇编形态（objdump）：
//   mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je +0x54     ← p.second 判空
//   取 vptr; add $0xc; call *(槽3)                              ← clearScriptData()
//   重读 p.second; test; je +0x4a                               ← 二次判空
//   取 vptr; add $0x4; call *(槽1)                              ← D0 deleting dtor
//   movl $0,0x4(pair)                                          ← p.second = 0
#include "Arad_Verify/operator.inc"

}  // namespace ARAD
