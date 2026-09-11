// ==== ORIG 085df012 ====
// [L10b P292 档①] int→bool（头同步）：ORIG PvP_Room::check_winner 0x085d9950
// `test %al,%al; je` 无 setne。
bool CDeathMatchBattleMgr::checkGameOverForSinglePlay(int mode)
{
    if (mode == 0)
        return 1;
    // ORIG 跳转表 0x8cc1430：mode 2/3 同判 >1，4→>2，5→>3，6→>4，7→>5，8→>6。
    // case 按降序书写以对齐 ORIG 基本块布局（>6 块在最低地址）。
    switch (mode)
    {
    case 8:
        if (getMaxKillCount() > 6)
            return 1;
        return 0;
    case 7:
        if (getMaxKillCount() > 5)
            return 1;
        return 0;
    case 6:
        if (getMaxKillCount() > 4)
            return 1;
        return 0;
    case 5:
        if (getMaxKillCount() > 3)
            return 1;
        return 0;
    case 4:
        if (getMaxKillCount() > 2)
            return 1;
        return 0;
    case 3:
    case 2:
        if (getMaxKillCount() > 1)
            return 1;
        return 0;
    default:
        return 1;   // mode 0/1 与 >8 均判结束
    }
}