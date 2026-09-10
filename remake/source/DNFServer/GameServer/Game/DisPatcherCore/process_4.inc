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
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_field14b8;
        req.m_12 =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_field14b7;
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

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const int lastPlay = user->GetAccountLastPlayTime();
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               lastPlay, curSec))
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
