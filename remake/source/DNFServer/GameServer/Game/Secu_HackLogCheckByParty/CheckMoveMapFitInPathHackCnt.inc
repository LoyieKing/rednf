void Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(
    CUser* user, CParty* party, unsigned int a, unsigned int b, int mapIdx)
{

    // P15/P16：ORIG 将 (auto||cnt==1)&&map&&!fit 结果物化为 0/1 再统一 test；
    // 参数判空次序按 ORIG：先 party(0x10) 后 user(0xc)。
    if (party == 0 || user == 0)
    {
    }
    else if (!party->IsFirstMapClear())
    {
        party->SetFirstMapClear(true);
    }
    else
    {
        CMap* map = (CMap*)G_CDataManager()->find_map(mapIdx);

        if ((party->IsAutoCreated() || party->get_member_count() == 1)
            && map != 0
            && !map->checkFitInPathObject(a, b))
        {
            LogManager::logFormat(
                1, "Secu_HackLogCheckByParty.cpp",
                __PRETTY_FUNCTION__,
                0x2c, "Path gate position error. (User: %s %d,%d)",
                NumberToString(user->get_acc_id(), 0), a, b);
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f7, 1, a, b);
        }
    }
}