void CLeagueMatch::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)  // ORIG T 0x0855d096
{
    mapNo = 3;
    if ((int)mode != 6)
        mode = static_cast<PVP_BATTLE_MODE>(nBattleMode_);
}