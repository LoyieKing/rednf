// ORIG RequestMap 0x8562bae：mapNo=0；mode!=6 时取 CDataManager::GetPvPMode。
void CFairMatch::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)
{
    mapNo = 0;
    if ((int)mode != 6)
    {
        mode = (PVP_BATTLE_MODE)G_CDataManager()->GetPvPMode();
    }
}