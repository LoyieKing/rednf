// ORIG 0x081ca2fe T process（process.md 全文转写）。
// err>0 返 0；err<0 LineFunc(0x1e5b)；err==0：世界图 hasDeathTower 死调用后，
// 未组队走 GameManager 单人 party（hack 记账 0x1f6 + set_single_play + game_start）；
// [b28d-29] N82：全帧槽号平移 4（ORIG 帧更浅），助记符序列与调用点全等  (ORIG 0x081ca2fe insn 129->133)
// 已组队且 state==3 时对所在 party game_start；均返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 129 / ours 132），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
int DisPatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    // ORIG +0x20：errNo store 双钉（技45）。
    // [b28d-solo-DisPatcherCore] P211：ORIG RHS-first+edx 中转（unsigned 返回 + 无具名局部）。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1e5b, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG +0x84 `movb $0x0,-0x15` dead bool 初始化（技8 变体）：hasDeathTower
    // 结果落 bool 槽；worldIdx 槽 -0x14。
    bool deathTower = false;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
    if (worldIdx > 0)
    {
        CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (worldMap != 0)
        {
            deathTower = worldMap->hasDeathTower();   // ORIG +0xd3 `mov %al,-0x15`
        }
    }
    (void)deathTower;

    // [G5-ProcB] ORIG 0x081ca3d4 `movl $0x0,-0x10(%ebp)`（hasDeathTower 之后、
    // CheckInParty 之前）＋ 两个分支各 `mov %eax,-0x10(%ebp)`：原码是函数级
    // `CParty* party = 0;` 两个分支共用同一槽（旧文在两块内各声明一次 ⇒ 缺这条
    // 0 初始化且槽序/指令数偏移）。
    CParty* party = 0;

    // ORIG +0xe8 `test %al,%al; je +0x134`：inParty 体是 fall-through 块（技17/19），
    // `get_state()==3` 为 cmp+sete+test 物化（技9 直写 ==3 已实证产 sete）。
    // [J5-ProcD 2026-09-11] ORIG 0x081ca3fd/+0x134 与 0x081ca4ea/+0x1ec 布局证据：
    //  - inParty 真块尾 ORIG `jmp +0x1f1`（0x081ca42d）**无** `mov $0,%eax` ⇒ 原码无块内
    //    `return 0;`（旧形 `{...; return 0;}` 多 1 条提前物化返回值）；
    //  - `party == 0` 分支 ORIG 是 `je +0x1c0`（0x081ca446，正向跳到远端 logFormat 块），
    //    hack/set_single_play/game_start 为 near fall-through 块、尾 `jmp +0x1ec`
    //    （0x081ca4bc），logFormat 段后**直接落共享尾** 0x081ca4ea `mov $0,%eax`
    //    ⇒ 原码是 `if (party != 0) {hack…} else {logFormat…}`（非 `party==0` 早返回，
    //    旧形产 `jne` + 近端 logFormat + `mov $0,%eax; jmp` 多 2 条）。
    // 改前形：`if (CheckInParty()) {…; return 0;} {p=GetParty(); if (p==0){log;return 0;} …}`
    // 改后形：`if (CheckInParty()) {…} else {p=GetParty(); if (p!=0){…} else {log;}}` + 尾单一 return 0。
    // 预期方向：负 gap（ours 133 → 131，ORIG 129）。
    if (user->CheckInParty())
    {
        if (user->get_state() == 3)
        {
            party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->game_start(user);
            }
        }
    }
    else
    {
        party = G_CGameManager()->GetParty();
        if (party != 0)
        {
            if (excludeAddHackCount(user))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x1f6, 1u, 0u, 0u);
            }
            party->set_single_play(user);
            party->game_start(user);
        }
        else
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x1e99,
                                  "cant party memory : you must free party memory!!!");
        }
    }
    return 0;
}
