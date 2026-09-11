// ============================================================================
// addUserHackCount150(CUser*) —— ORIG T 0x830dce4 [_Z19addUserHackCount150P5CUser]
// 同 149，仅 hack type=0x199。
// ============================================================================
bool addUserHackCount150(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)0x199, 1u, 0u, 0u);
    return true;
}