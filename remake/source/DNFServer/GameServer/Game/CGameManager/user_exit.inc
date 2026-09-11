// ORIG 0x82985a8 _ZN12CGameManager9user_exitEP5CUser：主动登出路径。
// 与 user_disconnect 的差异：state==0 视为异常直接拒绝；state<3 分支
// UpdateLogout(false)（disconnect 为 true）；无 DeleteRentalItemInfo；
// state>=3 分支末尾 ResetEpoll + checkLogOutCorrectly。
bool CGameManager::user_exit(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=222 our=219 gap=3；ndiff 81；P227 临时链
    // 已上无改善）。ORIG 0x082985a8。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR；trace 槽 -0x20 vs ORIG -0x3c
    // 等分支内槽组分区差）。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] verdict = **AE** 222/224：唯一非地址
    //   差 = logFormat 段后我方多 1 条 4.4 布局 nop（ORIG 0x082988a2 后无）⇒ §6.1
    //   豁免；数据地址差由 AE 归一口径吸收 ⇒ 本件已达上限。p167 CLEAR 44=44。
    if (user == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1271, 5);
        tr("USER_EXIT [%s][%d]", __PRETTY_FUNCTION__, 0x1271);
        return false;
    }
    if (user->get_state() == 0)
    {
        // ORIG 0x08298625：get_acc_id 结果先入 %ebx 跨 ctor 保活（无具名槽）⇒ P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x127a, 5)(
            "USER_EXIT STATE NONE [%s][%d]mid(%d)",
            __PRETTY_FUNCTION__, 0x127a, user->get_acc_id());
        return false;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);   // ORIG *(user+0x796f8)
    user->prepareDisconnect();
    G_GameWorld()->EraseLoginUser(user);
    // ORIG 0x082986b6：`cmp $0x2,%eax; setg %al; test %al; je <尾部else块>` —— 真条件
    // 是 state>2（CheckOut 族 fall-through，SetSaveRental 块落函数尾），Ghidra C 的
    // `iVar2 < 3` 块序误读；必须按 ORIG 极性写 >2 才复现（P59/P222）。
    if (user->get_state() > 2)
    {
        CheckOutTrade(user);
        CheckOutParty(user, true);
        CheckOutPvp(user, false);
        CheckOutWarRoom(user);
        checkOutDeathTower(user);
        checkOutBossTower(user);
        checkOutAdvanceAltar(user);
        checkOutBlueMarble(user);
        G_GameWorld()->leave_game_world(user);
        user->SetGameMasterMode(false);
        user->UpdateData();
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
        // ORIG 0x082987c2/0x082987d0：get_unique_id→%esi、getCurCharacName→%ebx
        // 在 ctor 前求值保活 ⇒ P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x12dc, 0)(
            "ID: %s - UID: %d from Logout",
            user->getCurCharacName(), user->get_unique_id() & 0xffff);
        // ORIG 0x08298815：err 先预清零（movl $0x0,-0xc）再独立赋值（P198①/P197③ 两句式）。
        int err = 0;
        err = G_EpollHandler()->ResetEpoll(user->m_network.get_socket());
        // ORIG 0x08298842：`cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je` ⇒ 4.1 的
        // bool 物化四段形，用 while+break 复现（P225）。
        while (err != 0)
        {
            LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x12e3,
                                  "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                                  NumberToString(user->get_acc_id(), 0),
                                  user->m_network.get_socket(), err, strerror(err));
            break;
        }
        user->ResetCurCharac();
        user->doLinkCharacDisconnect();
        returnUserPool(user);
        user->checkLogOutCorrectly();
    }
    else
    {
        user->SetSaveRentalInfoToExchange(false);
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
    }
    return true;
}