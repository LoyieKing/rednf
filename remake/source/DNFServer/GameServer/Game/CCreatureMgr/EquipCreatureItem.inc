// ORIG _ZN13user_creature12CCreatureMgr17EquipCreatureItemEPNS_9CCreatureES2_
// T 0x083393a2：非空旧宠先 Dismantle，非空新宠再 Equip，任一失败返回 0。
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
}
