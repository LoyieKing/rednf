bool CCreatureMgr::DismantleArtifact(int slot, Inven_Item* item)
{
    CArtifact* artifact = GetArtifact(slot);
    if (artifact == 0)
        return false;
    artifact->Reset();
    return true;
