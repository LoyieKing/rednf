int DisPatcher_ReportClient4Hack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG +0x40：重新 call get_state 取 %eax 后 4.1 switch 二分下探
    // （`cmp $6;je; cmp $6;jg; cmp $5;je; jmp d; cmp $7;je; cmp $8;je; jmp d`，
    // 技34 直写 switch 复现）；case5/7/8 校验失败走 LineFunc 岛，成功 break 共
    // 尾 analyze（+0x1c7 汇合）；default 走全链后直接 return 0。
    switch (user->get_state())
    {
    case 5:
        if (user->GetParty() == 0)
        {
            return LineFunc(0x9bb7, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 7:
        if (G_CGameManager()->GetPvp((int)user->GetPvpIndex(), user, 0) == 0)
        {
            return LineFunc(0x9bc1, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 8:
        if (user->GetWarRoom() == 0)
        {
            return LineFunc(0x9bc5, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 6:
        break;
    default:
        user->getHackAnalyzer()->beginCollectHackInfo(user);
        user->getHackAnalyzer()->analyzeHackReport(user, &packet);
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        return 0;
    }
    user->getHackAnalyzer()->analyzeHackReport(user, &packet);
    return 0;
