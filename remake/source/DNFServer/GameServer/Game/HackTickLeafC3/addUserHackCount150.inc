bool addUserHackCount150(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)0x199, 1u, 0u, 0u);
    return true;
