// ============================================================================
// OnlyOrigBatch11 落地：_NS_HS_NoResponseToHackLog —— ORIG T 0x08274f31
// user->getHackAnalyzer()->addServerHackCnt(user, 0x33c, 1, 0, 0)
// （addServerHackCnt 五参：CUser*, ENUM_HACKTYPE, uint, uint, uint）。
// ============================================================================
void _NS_HS_NoResponseToHackLog(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(0x33c), 1, 0, 0);
}