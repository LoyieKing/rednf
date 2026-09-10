bool CCreatureMgr::EquipCreatureItem(CCreature* dismantleItem,
                                     CCreature* equipItem)
{
    if (dismantleItem != 0)
    {
        if (!Dismantle(dismantleItem))
            return false;
    }
    if (equipItem != 0)
    {
        if (!Equip(equipItem))
            return false;
    }
    return true;
