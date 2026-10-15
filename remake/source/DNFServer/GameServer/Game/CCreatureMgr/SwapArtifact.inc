// ORIG _ZN13user_creature12CCreatureMgr12SwapArtifactEP10Inven_ItemiiS2_ii
// T 0x08339752：任一侧计数为 0 时按神器槽位拆解/装配，随后清空源物品首字节。
bool CCreatureMgr::SwapArtifact(Inven_Item* item1, int slot1, int type1,
                                Inven_Item* item2, int slot2, int type2)
{
    if (type1 == 0 || type2 == 0)
    {
        int equipSlot = -1;              // 待装配（槽号，物品指针）
        Inven_Item* equipItem = 0;
        int dismantleSlot = -1;          // 待拆解（槽号，物品指针）
        Inven_Item* dismantleItem = 0;

        if (IsArtifactEquipSlot(slot1))
        {
            if (!item2->isEmpty())
            {
                equipSlot = slot1;
                equipItem = item2;
            }
            if (!item1->isEmpty())
            {
                dismantleSlot = slot1;
                dismantleItem = item1;
            }
        }
        if (IsArtifactEquipSlot(slot2))
        {
            if (!item1->isEmpty())
            {
                equipSlot = slot2;
                equipItem = item1;
            }
            if (!item2->isEmpty())
            {
                dismantleSlot = slot2;
                dismantleItem = item2;
            }
        }

        if (dismantleSlot != -1 && dismantleItem != 0)
        {
            if (!DismantleArtifact(dismantleSlot, dismantleItem))
                return false;
        }
        if (equipSlot != -1 && equipItem != 0)
        {
            if (!EquipArtifact(equipSlot, equipItem))
                return false;
            equipItem->m_seal = 0;     // ORIG movb $0,(源物品)：仅清首字节
        }
    }
    return true;
}
