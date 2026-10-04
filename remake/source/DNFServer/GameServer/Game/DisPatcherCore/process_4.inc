// ORIG 0x081c6380 T _ZN23DisPatcher_SelectCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x1587,pretty,err,acc_id)。
// resetSaveFlags → 视图状态/上次登录判定 → 综合PvP频道抑制 → exchange 命中计数 →
// 回填 MSG+0xe/+0x12（_Charac_info.m_tagCharacNo/m_strikerSkillIndex）→ 九路 DB/ES 请求 →
// [b28d-29] N82：帧 0x1a0 vs 0x1b4 + 槽平移，调用点全等  (ORIG 0x081c6380 insn 320->323)
// 清上次登录 → 死塔最佳记录/注册日期 DB 请求 → LimitNpcBuyItemInfoAll 监视包。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 283（ORIG 320 / ours 323），残差=N7/N82 帧与 callee-saved 角色/P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽；沿用上方 b28d-29 归因，封顶。
// [H5DisPatcher 核账] 2026-09-11 实测 ours 323→326（+3）劣化 ⇒ 已回滚留 N。
int DisPatcher_SelectCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stSelectCharac& out = static_cast<stSelectCharac&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1587,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }

    // [G5-ProcB] ORIG 0x081c638e param 槽 -0x10（函数头）→ 0x081c6405
    // `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：msg 视图具名局部在两级 errNo
    // 检查之后、resetSaveFlags() 之前初始化（函数头声明会多一条 store 且槽基下移）。
    MSG_CHARAC_SELECT& req = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);

    user->resetSaveFlags();
    if (user->GetCharacViewState(static_cast<char>(req.m_slot)) == 1)
    {
        user->SetValidLastLoginCharac(false);
        user->SetCharacViewState(static_cast<char>(req.m_slot), 0);
        user->SetLoadRentalInfoFromExchange(false);
    }
    else
    {
        if (user->get_charac_no(static_cast<char>(req.m_slot)) ==
            user->GetLastLoginCharacNo())
        {
            const bool valid = (user->GetLastLoginCharacNo() != 0 &&
                                user->GetLastLoginChannelNo() != 0);
            if (valid)
            {
                user->SetValidLastLoginCharac(true);
                user->SetLoadRentalInfoFromExchange(true);
            }
        }
    }

    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        user->SetValidLastLoginCharac(false);
    }

    if (user->IsValidLastLoginCharac())
    {
        exchange_server::GetInstanceExchangeServerHitRate()->IncrementCacheLoadCount();
    }
    else
    {
        exchange_server::GetInstanceExchangeServerHitRate()->IncrementDBLoadCount();
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        req.m_0e = 0;
        req.m_12 = 0;
    }
    else
    {
        // P48：O0 无 CSE，ORIG 两字段各触发一次 vector::operator[] 调用（禁缓存 info）。
        req.m_0e =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_tagCharacNo;
        req.m_12 =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_strikerSkillIndex;
    }

    RequestDBCharac(user, req);
    RequestBlackCount(user, req);
    RequestDBInventory(user, req);
    RequestDBQuest(user, req);
    RequestDBDungeonClear(user, req);
    APSystem::CDBAndExchangeServerProc::RequestLoadActionPointFromDBAndES(*user, req);
    RequestDBKillMonsterInfo(user, req);
    advancealtar::DBAndExchangeServerProc::requestLoadFromDBAndES(*user, req);
    RequestDBETC(user, req);

    user->SetValidLastLoginCharac(false);
    user->SetLastLoginCharacNo(0);
    user->SetLastLoginChannelNo(0);

    DB_GetDeathTowerBestRecord::makeRequest(
        user->GetUID(), user->get_charac_no(static_cast<char>(req.m_slot)));

    // [J5-ProcD 2026-09-11 ndiff 481→451 落地] P0/P183：ORIG 0x081c64xx 段
    // `mov %eax,%esi`（第 3 实参 curSec）/ `mov %eax,%ebx`（第 2 实参 lastPlay）跨 call
    // 用 callee-saved 保活、末尾 `mov %esi,0x8(%esp); mov %ebx,0x4(%esp)` —— **无具名槽**；
    // 旧形两个 `const int` 具名局部使每值多 1 store+1 load（我方帧 0x1b4 vs ORIG 0x1a0）。
    // 父泳道核账：指令数不变但 ndiff **481→451**（net −30）⇒ 保留本形。
    // 改前形：`const int curSec = …; const int lastPlay = …; CheckDailyScheduleTime(…, lastPlay, curSec)`
    // 改后形：三实参内联（cdecl 右→左求值序与 ORIG 保活序一致）。
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               user->GetAccountLastPlayTime(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        DB_GetUserRegdate::makeRequest(user->GetUID(), user->get_acc_id());
    }

    // 监视包（ORIG 栈上构造，uid(+0xa)/acc_id(+0xe)/charac_no(+0x12) 后发 0x182 字节）
    LimitNpcBuyItemInfoAll monitorInfo;
    monitorInfo.m_padA = user->GetUID();
    monitorInfo.m_dbid = user->get_acc_id();
    monitorInfo.m_charNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<char>(req.m_slot)));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&monitorInfo), 0x182);
    return 0;
}
