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
    int itemId = item->m_addInfo;
    CItem* pItem = G_CDataManager()->find_item(itemId);
    if (pItem == 0)
        return false;
    if (pItem->is_stackable())
        return false;
    m_pItem = (CEquipItem*)pItem;
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
    if (m_pItem == 0)
        return 0.0f;
    if (m_pItem->is_stackable())
        return 0.0f;
    return m_pItem->get_experience_amount();
}

}  // namespace user_creature
