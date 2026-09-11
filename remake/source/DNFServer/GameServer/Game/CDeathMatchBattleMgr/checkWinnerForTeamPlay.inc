// ==== ORIG 085df296 ====
// [b28d-27] DIFF N82/N93：ORIG 0x085df296；p167 CLEAR（4/4）。
// [b28e-L6 全形回填（探针 /tmp/l6p/p5.cpp 逐条同构实证，帧 0x48/槽集全等）]
//   ① 两个循环共用**函数级 `int i`**（ORIG 序言只发一次 `movl $0x0,-0x14`，
//      第三循环亦复用 -0x14）⇒ 禁 for-init 内声明（4.4 会各开独立槽）。
//   ② 声明序 k1,k2,alive[2],i,t,winTeam ⇒ 4.4 槽 k1 -0x1c/k2 -0x18/
//      alive -0x24,-0x20/t -0x10/i -0x14/winTeam -0x9，与 ORIG 全等（P28/P331）。
//   ③ 第一循环 guard 负条件 goto 尾空标签（ORIG 0x085df2c3 `je 085df342(nop)`、
//      0x085df1f5/1f7 jmp+nop 岛）——4.4 正包裹恒 jump-thread。
//   ④ 第二循环 = 正包裹 `if (win[i] != 0)` + 内层 goto 岛串：ORIG 四出口为
//      0x085df374 `je 3c5`（直落 addl）/ 0x085df38e `je 3be(nop;jmp)` /
//      0x085df3a6 `js 3c1(nop;jmp)` / 0x085df3ac `jg 3c4(nop)`；4.4 只有此形给出
//      「1 直落 + 3 岛」组合（合并 &&/单 goto 形恒 0 岛或 4 岛）。
//   ⑤ `int t` 必须函数级（ORIG 0x085df364 `movl $0x0,-0x10` 与 ⑥ 同批发于循环后，
//      故写 `t = 0;` 赋值语句而非声明初值——声明初值会把该 store 提到函数头）。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
void CDeathMatchBattleMgr::checkWinnerForTeamPlay(
    CUser** users, bool* win, unsigned char* teamWin, PvpUserTable& table)
{
    int killTeam1 = 0;
    int killTeam2 = 0;
    // [本波·R20 建议] ORIG 0x085df296 用 `int winCount[2]`（基址 -0x24 + team 索引）
    // 而非两个独立标量 ⇒ 改数组形（索引访问形态与 ORIG 的 `-0x24(,%edx,4)` 一致）。
    // ⑥ 零初始化**位置**也是 ORIG 形状的一部分：0x085df356/35d/364 三条
    //    `movl $0x0,-0x24/-0x20/-0x10` 在**第一循环之后**才发射（函数头只有
    //    k1/k2/i 三条）⇒ 声明置函数头（保槽位 -0x24/-0x20/-0x10）而把初始化
    //    写成循环后的赋值语句（探针 /tmp/l6p/p7.cpp v_a 逐条同构）。
    int aliveTeam[2];
    int i;
    int t;
    unsigned char winTeam;
    for (i = 0; i <= 7; ++i)
    {
        // P223：负条件 goto 尾空标签复现 ORIG 的 nop 岛 + 分支尾 jmp。
        if (users[i] == 0)
            goto nextUser;
        if (teamWin[i] == 1)
            killTeam1 += m_killCount[i];
        else if (teamWin[i] == 2)
            killTeam2 += m_killCount[i];
        else
            LogManager::logFormat(
                1, "pvp.cpp",
                __PRETTY_FUNCTION__,
                0x1617,
                "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                i, (unsigned int)teamWin[i]);
    nextUser: ;
    }
    aliveTeam[0] = 0;
    aliveTeam[1] = 0;
    t = 0;
    for (i = 0; i <= 7; ++i)
    {
        if (win[i] != 0)
        {
            if (users[i] == 0)
                goto nextAlive;
            t = teamWin[i] - 1;
            if (t < 0)
                goto nextRange;
            if (t > 1)
                goto nextGrow;
            ++aliveTeam[t];
        nextGrow: ;
        nextRange: ;
        nextAlive: ;
        }
    }
    if (killTeam1 < killTeam2)
        winTeam = 2;
    else if (killTeam1 > killTeam2)
        winTeam = 1;
    else if (aliveTeam[0] > aliveTeam[1])
        winTeam = 1;
    else if (aliveTeam[1] > aliveTeam[0])
        winTeam = 2;
    else
        winTeam = (rand() & 1) ? 2 : 1;
    memset(win, 0, 8);
    for (i = 0; i <= 7; ++i)
    {
        if (teamWin[i] == winTeam)
            win[i] = true;
    }
    table.SetWinTeam((int)winTeam);
}