#include "CInventory.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "Inven_Item.h"

namespace user_creature
{

CArtifact::CArtifact()
{
    m_pItem = 0;
}

void CArtifact::Reset()
{
    m_pItem = 0;
}

bool CArtifact::EquipArtifact(const Inven_Item* item)
{
    if (item == 0)
        return false;
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
