// ORIG T 0x0860b2c2（报告 CSpecialItemRoutingManager/ProcessGiveUp.md）：
// party 非空时——路由未开启 或 全员完成选卡（HasAllMemberSelection）——
// 且首个有效成员状态为 5（短路返回）或再次读取为 0xc 时，执行 processRouting。
void CSpecialItemRoutingManager::ProcessGiveUp(CParty* party)
{
    if (party == 0)
        return;

    if (party->IsRoutingState() && party->HasAllMemberSelection()
        && (party->GetFirstValidMemberState() == 5
            || party->GetFirstValidMemberState() == 0xc))
        processRouting(party);
}