void break_away_prevent::CBreakAwayPreventSystem::updateUsedCera(unsigned int accId,
                                                                 unsigned int cera)
{
    DB_UpdateBreakAwayAccruedCera::makeRequest(accId, cera);
