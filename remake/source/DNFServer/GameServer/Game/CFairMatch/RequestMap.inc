void CFairMatch::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)
{
    mapNo = 0;
    if ((int)mode != 6)
    {
        mode = (PVP_BATTLE_MODE)G_CDataManager()->GetPvPMode();
    }
