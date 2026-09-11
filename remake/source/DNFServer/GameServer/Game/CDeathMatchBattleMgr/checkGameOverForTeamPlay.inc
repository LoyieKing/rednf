// ==== ORIG 085df126 ====
// SIGFIX b28c-L29：ORIG 串 "bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(...)"。
// [b28d-27] DIFF N82：ORIG 0x085df126；p167 CLEAR（1/1）。
// [b28e-L6 三旋钮回填（探针 /tmp/l6p/p1.cpp 实证）]
//   ① 0x085df178 `je 085df1f7(nop)` + 0x085df1f5 `jmp 085df1f8` ⇒ guard 需负条件
//      goto 尾空标签（P223）；4.4 正包裹 `if(a!=0||b!=0)` 恒 jump-thread 掉 jmp+nop。
//   ② 0x085df213/216 与 0x085df21b/21e **两臂各自** store ⇒ 原码是 if/else 两臂赋值；
//      三目 `?:` 被 4.4 折成共享 store（少 1 条 mov）。
//   ③ case 叶块物理序=书写序（P324）：ORIG 块地址升序 0x085df23f(4)→0x253(3)→0x267(2)
//      →0x27b(1)→0x28f(default) ⇒ case 必须 4,3,2,1 降序书写（4.4 决策树不变、
//      `cmp $4; jne` fall-through 形态与 ORIG 一致）。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(
    CUser** users, int teamCnt, unsigned char* teamWin)
{
    if (teamCnt == -1)
        return true;
    int killTeam1 = 0;
    int killTeam2 = 0;
    // [本波] ORIG 0x085df126 序言发三个零初始化 `movl $0x0,-0x18/-0x14/-0x10`
    // ⇒ maxKill 是**函数级具名 int（带 0 初值）**，三目赋值降为循环后的独立语句
    // （P28/P212 槽序：killTeam1 -0x18 → killTeam2 -0x14 → maxKill -0x10 → i -0xc）。
    int maxKill = 0;
    for (int i = 0; i <= 7; ++i)
    {
        // P223：负条件 goto 共享尾空标签复现 ORIG 的 nop 岛 + 分支尾 jmp。
        if (users[i] == 0 && m_killCount[i] == 0)
            goto nextUser;
        if (teamWin[i] == 1)
            killTeam1 += m_killCount[i];
        else if (teamWin[i] == 2)
            killTeam2 += m_killCount[i];
        else
            LogManager::logFormat(
                1, "pvp.cpp",
                __PRETTY_FUNCTION__,
                0x157f,
                "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                i, (unsigned int)teamWin[i]);
    nextUser: ;
    }
    if (killTeam1 > killTeam2)
        maxKill = killTeam1;
    else
        maxKill = killTeam2;
    // [本波·P56] ORIG 各 case 为 `cmpl $N,-0x10; jle <尾岛 mov $0>` + `mov $0x1,%eax`
    // 分支形（jle×4 + jmp×4）；直写 `return maxKill > N;` 被 4.4 折成 `setg` 单流 ⇒
    // 改 if-return 显式形。case 降序书写以对齐 ORIG 叶块物理序（见函数头③）。
    switch (teamCnt)
    {
    case 4:
        if (maxKill > 9)
            return true;
        return false;
    case 3:
        if (maxKill > 7)
            return true;
        return false;
    case 2:
        if (maxKill > 4)
            return true;
        return false;
    case 1:
        if (maxKill > 1)
            return true;
        return false;
    default:
        return true;
    }
}