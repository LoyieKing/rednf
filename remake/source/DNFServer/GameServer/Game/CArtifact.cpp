// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CArtifact/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CInventory.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "Inven_Item.h"

namespace user_creature
{

#include "CArtifact/CArtifact.inc"
{
    Reset();   // ORIG 0x833ef0c：ctor 仅调用 Reset
}

#include "CArtifact/Reset.inc"
{
    m_pItem = 0;
}

#include "CArtifact/EquipArtifact.inc"
{
    // ORIG 0x8336f6e：无 item 空指针检查，直接取 item+2（m_addInfo/item idx）。
    // ORIG push %ebx + `mov 0x2(%eax),%eax; mov %eax,%ebx`…`mov %ebx,0x4(%esp)`：
    // idx 值跨 G_CDataManager call 缓存 %ebx（P0 无槽往返）⇒ P197② 语句表达式
    // 实参形令 4.4 升格 callee-saved。
    // P211 同型化：局部直存 CEquipItem*（O0 指针 cast 零成本），使尾 store
    // `m_pItem = pItem` 字段/RHS 同型 ⇒ 4.4 LHS-first = ORIG 0x8336fb9 形。
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(
        ({ item->m_addInfo; }));
    if (pItem == 0)
        return false;
    if (pItem->is_stackable())
        return false;
    m_pItem = pItem;
    return true;
}

#include "CArtifact/GetCreatureMinimumLevel.inc"
{
    if (m_pItem == 0)
        return 0;
    if (m_pItem->is_stackable())
        return 0;
    return m_pItem->get_creature_minimum_level();
}

#include "CArtifact/GetExperienceAmount.inc"
{
    // [b28d-39] ORIG 0x08336fc8 帧 0x38、具名 float -0xc、返回物化槽 -0x1c；
    // 单一具名 float 局部即精确复现（旧 2×取址 double 撑帧使返回槽 -0x2c、帧 0x48，已删）。
    float amount;
    if (m_pItem == 0)
        return 0.0f;
    if (m_pItem->is_stackable())
        return 0.0f;
    // ORIG 0x8336ffc：call 结果 fstps -0xc（具名局部往返）→ 尾 `mov %eax,-0x1c;
    // flds -0x1c`（统一返回槽）。P0 槽往返 ⇒ 原码有具名 float 局部。
    amount = m_pItem->get_experience_amount();
    return amount;
}

}  // namespace user_creature
