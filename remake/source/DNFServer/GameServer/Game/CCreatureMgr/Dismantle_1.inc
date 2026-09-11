// ORIG _ZN13user_creature12CCreatureMgr9DismantleEPNS_9CCreatureE T 0x0833934c：
// 种族类型 1 才可卸下；成功后清空已装备记录。
bool CCreatureMgr::Dismantle(CCreature* creature)
{
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Dismantle(m_user))
        return false;
    m_equippedCreature = 0;
    return true;
}
