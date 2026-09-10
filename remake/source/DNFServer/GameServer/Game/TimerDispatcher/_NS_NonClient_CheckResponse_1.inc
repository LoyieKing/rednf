int _NS_NonClient_CheckResponse(int slot, int uniqueId, int characNo)
{
    CUser* user = G_CGameManager()->getUser(slot, 2);
    if (user == 0)
    {
        return 0;
    }
    if (user->get_unique_id() != uniqueId)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacNo() != characNo)
    {
        return 0;
    }
    if (!user->GetNonClientFlag())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x33a, 1, 0, 0);
    }
    return 1;
