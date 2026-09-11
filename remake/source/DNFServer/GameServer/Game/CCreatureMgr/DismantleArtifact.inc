// ORIG _ZN13user_creature12CCreatureMgr17DismantleArtifactEiP10Inven_Item T 0x08339718：
// GetArtifact 为空 → 0；否则 CArtifact::Reset 后返回 1。
bool CCreatureMgr::DismantleArtifact(int slot, Inven_Item* item)
{
    CArtifact* artifact = GetArtifact(slot);
    if (artifact == 0)
        return false;
    artifact->Reset();
    return true;
}
