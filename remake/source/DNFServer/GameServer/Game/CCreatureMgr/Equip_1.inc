// ORIG _ZN13user_creature12CCreatureMgr5EquipEPNS_9CCreatureE T 0x083392f6：
// 仅种族类型 1 可装备；CCreature::Equip 成功后记录为已装备宠。
bool CCreatureMgr::Equip(CCreature* creature)
{
    // ORIG 经外联 GetCreatureItemType(W 0x833ef98) 取类型后 !=1 判定（无中间局部）
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Equip(m_user))
        return false;
    m_equippedCreature = creature;
    return true;
}
