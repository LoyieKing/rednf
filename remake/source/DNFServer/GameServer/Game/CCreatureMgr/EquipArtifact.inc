// ORIG _ZN13user_creature12CCreatureMgr11EquipArtifactEiP10Inven_Item T 0x083396c8：
// GetArtifact 为空 → 0；否则委托 CArtifact::EquipArtifact 并按其结果返回。
bool CCreatureMgr::EquipArtifact(int slot, Inven_Item* item)
{
    CArtifact* artifact = GetArtifact(slot);
    if (artifact == 0)
        return false;
    if (!artifact->EquipArtifact(item))
        return false;
    return true;
}
