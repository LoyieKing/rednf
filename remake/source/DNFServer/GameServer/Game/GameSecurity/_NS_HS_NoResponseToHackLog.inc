void _NS_HS_NoResponseToHackLog(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(0x33c), 1, 0, 0);
