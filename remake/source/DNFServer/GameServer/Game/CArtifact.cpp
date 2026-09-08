// ============================================================================
// df_game_r 还原 —— user_creature::CArtifact（4B 单指针类，权威 TU）
// ORIG 符号（nm -S --defined-only 实证）：
//   W 0x833ef0c C1 ctor（唯一动作：call Reset）
//   T 0x8336f60 Reset
//   T 0x8336f6e EquipArtifact(Inven_Item const*)
//   T 0x83336fc8 GetExperienceAmount
//   T 0x83337012 GetCreatureMinimumLevel
// 无 dtor。报告：docs/class_func_reports/user_creature__CArtifact/。
// ============================================================================
#include "CInventory.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "Inven_Item.h"

namespace user_creature
{

CArtifact::CArtifact()
{
    Reset();   // ORIG 0x833ef0c：ctor 仅调用 Reset
}

void CArtifact::Reset()
{
    m_pItem = 0;
}

bool CArtifact::EquipArtifact(const Inven_Item* item)
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

int CArtifact::GetCreatureMinimumLevel()
{
    if (m_pItem == 0)
        return 0;
    if (m_pItem->is_stackable())
        return 0;
    return m_pItem->get_creature_minimum_level();
}

float CArtifact::GetExperienceAmount()
{
    // P190 附注① + P28 声明序：ORIG 帧 0x38、具名 float -0xc、返回物化槽 -0x1c；
    // amount 先声明（浅槽 -0xc），2 个取址 double 撑帧并垫深区。
    float amount;
    double framePad1;
    double framePad2;
    (void)&framePad1;
    (void)&framePad2;
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
