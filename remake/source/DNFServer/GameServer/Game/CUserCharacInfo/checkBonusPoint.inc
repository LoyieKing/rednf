void CUserCharacInfo::checkBonusPoint()
{
    if (!m_selected)
        return;
    resetBonusPoint();
    const Inven_Item* invenItem = 0;
    for (int slot = 10; slot <= 0x15; ++slot)
    {
        if ((invenItem = getCurCharacInvenR()->GetInvenRef(0, slot)) == 0)
            continue;
        if (invenItem->m_addInfo == 0)
            continue;
        CItem* found = G_CDataManager()->find_item(invenItem->m_addInfo);
        if (found == 0)
            continue;
        CEquipItem* equip = (CEquipItem*)found;
        if (equip->getWpBonusPoint()->m_type == 0)
        {
            CUR->m_wpBonus.m_base += equip->getWpBonusPoint()->m_value;
        }
        else if (equip->getWpBonusPoint()->m_type == 1)
        {
            CUR->m_wpBonus.m_add += equip->getWpBonusPoint()->m_value;
        }
        if (equip->getExpBonusPoint()->m_type == 0)
        {
            CUR->m_expBonus.m_base += equip->getExpBonusPoint()->m_value;
        }
        else if (equip->getExpBonusPoint()->m_type == 1)
        {
            CUR->m_expBonus.m_add += equip->getExpBonusPoint()->m_value;
        }
    }
