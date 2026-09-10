void CHackAnalyzer::ReqDBSaveHackUserPunish(unsigned int accId,
                                            unsigned int punish)
{
    // ORIG T 0x080f906a（ReqDBSaveHackUserPunish.md）：直发 DB 请求（ip=NULL）。
    DB_ReqSaveHackUserPunish::makeRequest(accId, punish, (char const*)0);
