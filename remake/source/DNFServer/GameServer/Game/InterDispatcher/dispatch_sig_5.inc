int Inter_LoadCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadCharacPktView* pPkt = reinterpret_cast<Inter_LoadCharacPktView*>(data);
    if (user->get_state() == 0)                        // state==0 → 0x13dd
        return 0x13dd;
    if (pPkt->m_8 != 0)        // req field @+0x8 → 0x13e3
        return 0x13e3;

    // [K3 负结论 2026-09-11] 试删 curAccId/reqAccId 两具名槽改内联重读：我方 680→681 条
    //   （+1）⇒ 4.4 下重读形比寄存器保活更贵，已回滚。
    const unsigned int curAccId = (unsigned int)user->get_acc_id();
    const unsigned int reqAccId =                      // msg+0x140
        pPkt->m_140;
    if (curAccId != reqAccId)
    {   // 异号 MID 记账：SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x13f9);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x9d);
        guard.operator*()->operator<<(-1);
        SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK* sig =
            guard->GetInBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>();
        sig->m_curAccId = static_cast<unsigned int>(user->get_acc_id());
        sig->m_reqAccId = reqAccId;
        sig->m_characNo = pPkt->m_c;
        user->m_network.GetPeerIP2(sig->m_peerIp, 0x14);   // +0x0c
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        return 0x1405;
    }

    if (pPkt->m_2e == '\0')  // charac list 空检查
        return 0x140b;

    const unsigned int reqCharacNo =                   // msg+0x04
        pPkt->m_4;
    const int slotIdx = user->get_charac_idx(reqCharacNo);
    if (slotIdx < 0 ||
        !user->setCharacInfoDetail(slotIdx, reqCharacNo,
                                   // P248：ORIG 084c34b9 `lea 0xc(%eax),%edx`（disp 形）。
                                   reinterpret_cast<CHARAC_DATA*>(
                                       &((Inter_LoadCharacAt0cViewI*)data)->m_c)))
        return 0x1415;

    unsigned int tagCharacNo =                         // msg+0x686
        pPkt->m_686;
    int tagIdx = user->get_charac_idx(tagCharacNo);
    if (tagCharacNo != 0 && tagIdx != -1)
    {
        if (tagIdx < 0 ||
            !user->setTagCharacInfo(tagIdx, tagCharacNo,
                                    // P248：ORIG 报告 +0x1cb `lea 0x68b(%eax),%edx`。
                                    reinterpret_cast<TAG_CHARAC_DATA*>(
                                        &((Inter_LoadCharacAt68bViewI*)data)->m_68b)))
            return 0x141f;
        // +0x14b7 strikerSkillIndex（_Charac_info::m_field14b7，CUser.h:719 权威成员；
        //   ORIG 084c364e `mov %dl,0x14b7(%eax)` 单条 disp 直证）。
        user->m_characList[slotIdx].m_field14b7 = pPkt->m_68a;
    }
    user->SelectCharac(slotIdx, tagIdx);

    const int characCount = user->get_charac_count();
    user->decideLinkCharacSlotIndexes((char)slotIdx, characCount);
    if (!user->checkCharacLinkType((char)slotIdx, user->get_charac_count()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1430, 5)("LINK_LOG : ABNOMAL LINK CHARAC!! m_id (%s), slot_index(%d)",
              NumberToString((unsigned int)user->get_acc_id(), 0), slotIdx);
        return 0x1431;
    }

    if (user->isCharacLinkBonusSetting((char)slotIdx))
        user->setCharacLinkBonusSetting(slotIdx, false);
    user->SetCurCharacEmotion(
        pPkt->m_128);
    {   // 扩展槽状态域 msg+0x12b：<0 或 >3 归零
        const signed char exSlot = pPkt->m_12b;
        // ORIG 两个独立 SetCurExpEquipSlotStat 调用点（越界 → 0，否则 → 原值）。
        if (exSlot < 0 || exSlot > 3)
            user->SetCurExpEquipSlotStat(0);
        else
            user->SetCurExpEquipSlotStat(exSlot);
    }
    {   // 公会代理信息拉取
        // ORIG get_charac_guildkey 两次独立调用（非 GM 门判 + 代理查找实参）。
        if (user->get_charac_guildkey() != 0)
        {
            STGuildProxy proxy;                        // ORIG 默认构造局部
            CGuildServerProxy* proxyMgr =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    user->GetServerGroup());
            if (proxyMgr->GetGuildInfoProxy(
                    user->get_charac_guildkey(), proxy))
                user->SetGuildInfoProxy(proxy);
        }
    }
    user->setCurCharacHelpAbuseComputedRatio(
        static_cast<unsigned int>(user->calc_disp_help_abuse_ratio(0)));
    user->EnableCharacInfo(CACHE_CHARAC);          // ORIG 0x84c386b（calc 后、ReqDBLoadMail 前）
    WongWork::CMailBoxHelper::ReqDBLoadMail(user, reqCharacNo);
    if (!G_GameWorld()->InsertUserToIDMap(data + 0x10,
                                          (unsigned int)user->get_acc_id()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1464, 5)("Insert into IDMap Failed %s [%s]",
              data + 0x10,
              NumberToString((unsigned int)user->get_acc_id(), 0));
    }

    // ---- 当日回访/幽灵状态判定 ----
    const long nowSec = GlobalData::s_systemTime_.getCurSec();
    time_t tNow = nowSec;
    struct tm today;
    struct tm* pToday = localtime_r(&tNow, &today);
    const int todayYear = pToday->tm_year;
    const int todayMon = pToday->tm_mon;
    const int todayMday = pToday->tm_mday;

    if ((long)nowSec >= user->GetCurCharacChaosResponTime())
    {
        // ORIG 0x84c3963：responTime <= now → ghost=false 并清零计时。
        user->SetCurCharacGhost(false);
        user->SetCurCharacChaosDieTime(0);
        user->SetCurCharacChaosResponTime(0);
    }
    else
    {
        // ORIG 0x84c39b2：ghost=true；dieLeft=responTime-dieTime、
        // responLeft=responTime-now，NotifyGhostTime(user, dieLeft, responLeft)。
        user->SetCurCharacGhost(true);
        const int dieLeft = static_cast<int>(
            user->GetCurCharacChaosResponTime() -
            user->GetCurCharacChaosDieTime());
        const int responLeft = static_cast<int>(
            user->GetCurCharacChaosResponTime() - nowSec);
        pvp_assault::CAssaultMgr::NotifyGhostTime(user, dieLeft, responLeft);
    }
    user->SetCurCharacLeaveTimeout(false);

    {
        const long lastPlayTick = user->getCurCharacLastPlayTick();
        if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                   lastPlayTick,
                                   GlobalData::s_systemTime_.getCurSec()))
        {
            user->setCurCharacTradeGoldDaily(0);
        }
        user->SetCurCharacLuckPointUpdateTimeCount(0);
    }
    {
        time_t lastPlay = user->getCurCharacLastPlayTick();
        struct tm lastDay;
        struct tm* pLast = localtime_r(&lastPlay, &lastDay);
        if (pLast->tm_year != todayYear || pLast->tm_mon != todayMon ||
            pLast->tm_mday != todayMday)
        {
            GetInstanceValueStatistic()
                ->AddValueStatistic((VALUE_STATISTIC_FIELD)0, user, 1);
        }
    }

    int ComputeDaysScheduleTime(int hour, long lastTime, long curTime);  // ORIG 0x846c144（GameUtils.cpp）
    unsigned long StringToNumber(const char* s);          // ORIG T 0x081090fa（DNFFunctionLib.cpp 定义）

    {   // antibot 用户信息上报（vtable+0x38 槽，类未建模——asm 桥残留保留，按 §8 协议待类建模后清理）
        user->fillUserInfo4Antibot();
        const char* userInfo = user->getUserInfo4Antibot();
        unsigned char payload[0x48];   // ORIG local_150/local_14c/acStack_148 连续区
        *reinterpret_cast<int*>(&payload[0]) =
            static_cast<int>(StringToNumber(user->get_acc_name()));
        *reinterpret_cast<int*>(&payload[4]) = -1;
        strncpy(reinterpret_cast<char*>(&payload[8]), userInfo, 0x40);
        typedef void (*ReportFn)(void*, int, int, int, void*, int);
        void* checker = GlobalData::s_antibotChecker_;
        ReportFn report = *(ReportFn*)((*(void***)checker)[0x38 / 4]);
        // [G00 2026-09-12 · 值级信号轴真缺口修复] ORIG 0x84c3b86
        //   `movl $0x186a1,0xc(%esp)` ⇒ 第 4 实参 = **100001**（我方旧值 100000，差 1；
        //   由 G19 在值级信号扫描中逐条对齐两侧 objdump 同上下文确证）。
        //   用具名 enum 常量（不占栈槽 ⇒ 不触发 P0 槽律副作用）。
        enum { kAntibotReportValueLimit = 100001 };   // ORIG 0x84c3b86 movl $0x186a1
        report(checker, G_CEnvironment()->getProcessSequence(), 0,
               kAntibotReportValueLimit, &payload[0], 0x48);
    }
    {   // PowerWar 未连接天数扣点链（lastPlayTickPowerWar = curCharac +0x7b long）
        // ORIG 4 次 `_ZNK getCurCharacR`（门判 + 读 +0x7b + trace 两处）、
        // 2 次 getCurSec（扣点判定 + trace）。+0x7b 直接 long 读，非 memcpy。
        if (*reinterpret_cast<const int*>(
                reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b) != 0)
        {
            const long cur = GlobalData::s_systemTime_.getCurSec();
            const long lastPlayTickPw =
                *reinterpret_cast<const long*>(
                    reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b);
            const int days = ComputeDaysScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime, lastPlayTickPw, cur);
            // ORIG 读 CDataManager +0xa6b4（powerwar point per day；
            // 暂无具名成员 [blocked: CDataManager 布局批补名]）
            const int perDayPoint =
                *reinterpret_cast<const int*>(
                    reinterpret_cast<const char*>(G_CDataManager()) + 0xa6b4);
            if (days > 0)
            {
                user->DecreasePowerWarPoint((short)((short)perDayPoint * (short)days));
                cMyTrace(__PRETTY_FUNCTION__,
                    0x14e4, 0)("POWERWAR POINT DECREASE : user:%d, last_play_tme:%d, "
                      "current_time:%d, Unconnected Days:%d, Per Day Point : %d, "
                      "Decrease powerwar point:%d",
                      *reinterpret_cast<const int*>(
                          reinterpret_cast<const char*>(user->getCurCharacR())),
                      *reinterpret_cast<const int*>(
                          reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b),
                      (int)GlobalData::s_systemTime_.getCurSec(),
                      days, perDayPoint, days * perDayPoint);
            }
        }
    }

    if (GlobalData::s_power_manager->IsPowerWarEventOn() == false)
    {
        if (user->GetPowerWarAssaultCount() != 0 ||
            user->GetPowerWarAssaultVictoryCount() != 0)
        {
            user->ResetPowerWarAssaultRecord();
        }
    }

    EventClassify::CEventScriptMng* eventMng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    if (eventMng->is_eventing(0x19b))
    {
        user->setCurUseBringUpMaterialTotal(pPkt->m_139);
        user->setBringUpRewardFlag(pPkt->m_138 ? true : false);
        user->SendEventBringUpInfo();
    }
    user->select_charac_set_visible_values(pPkt->m_13a);
    user->decide_growth_power_reward_system();
    return 0;
}