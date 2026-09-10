void CUser::FatigueUp(int v)
{
    // Hades+GoldControl 双扣减；state==5 时混沌点扣减与累计(+0x8e064)；
    // +0x8e08c=v；prev 疲劳逢 50 上报 hack 0x263；0x9c/0x32/0x64 清动作组
    // 0x16/0x28/0x2a；事件脚本 inc/gift/bring_up(0x19b)/process_add_fatigue；
    // 学园点；英雄任务 MissionNo=2；repeat 事件 0x29(角色日)/0x2b(疲劳buff)；
    // AvatarRechargeServer 计耐久；+0x79642 累计；公会 temp exp；上限/燃烧/
    // premium 分支（详见 docs/class_func_reports/CUser/FatigueUp.md）。
    if (v == 0)
        v = 1;
    unsigned short prevFatigue =
        (unsigned short)getCurCharacUsedFatigue();

    // ORIG 0x08655c88：getUserBreakAwayRewardLuckPoint 仅调一次，结果缓存到槽
    // 供 IncCurCharacLuckPoint 复用（asm +0x033 mov %eax,-0xc(%ebp) + +0x03f 重载）。
    // 逗号操作符短路形匹配 ORIG `iVar8=getUserBreakAwayRewardLuckPoint(),0<iVar8`。
    int luckPoint;
    if (getCurCharacR() != 0 &&
        (luckPoint = getUserBreakAwayRewardLuckPoint(), luckPoint > 0))
    {
        IncCurCharacLuckPoint(luckPoint);
        setUserBreakAwayRewardLuckPoint(0);
        setUsedBreakAwayLuckPoint(true);
    }

    // XNuclear::CHades::UseFatigue（ORIG T 0x084b9552；权威定义见 CHades.cpp）
    ((XNuclear::CHades*)getHades())->UseFatigue((unsigned int)v);
    GetGoldControl()->UseFatigue(v);

    if (get_state() == 5 && getCurCharacR() != 0)
    {
        IncCurCharacAssaultFatigueCount();
        if (GetCurCharacAssaultFatigueCount() > 1)
        {
            SubCurCharacChaosPoint(1);
            SetCurCharacAssaultFatigueCount(0);
            *(int*)((char*)this + 0x8e064) += v;   // ORIG 混沌疲劳累计
        }
    }
    m_fatigueGain = v;                             // +0x8e08c
    incCurCharacUsedFatigue((unsigned short)v);
    unsigned short nowFatigue = (unsigned short)getCurCharacUsedFatigue();
    // ORIG：imul $0x147b（除以 25 魔术数）后乘 -0x2a（42）——ORIG 实际为
    // now - 42*floor(now/50) == 0（仅 now==0 触发）；NEW 曾误写 -0x32(50)。
    if ((short)((short)nowFatigue + (short)((nowFatigue >> 1 & 0x7fff) / 0x19) * -0x2a) == 0)
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x263, 1, 0, 0);

    if ((short)getCurCharacUsedFatigue() == 0x9c)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x16, 0, (ENUM_PACKETCLASS)0);
    if ((short)getCurCharacUsedFatigue() == 0x32)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x28, 0, (ENUM_PACKETCLASS)0);
    else if ((short)getCurCharacUsedFatigue() == 100)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x2a, 0, (ENUM_PACKETCLASS)0);

    // ---- 事件脚本（EventClassify::CEventScriptMng）----
    EventClassify::CEventScriptMng* esm = static_cast<
        EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    esm->inc_user_fatigue(this, (unsigned int)v);
    esm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    esm->send_gift_fatigue_item(this, (unsigned int)v);
    esm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    if (esm->is_eventing(0x19b))
    {
        esm = static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
        esm->send_bring_up_item_fatigue(this, v);
    }

    if (isJoinedSchoolPointEvent())
        calcCurCharacSchoolPoint(prevFatigue);

    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, (HeroMissionCondition::MissionNo::T)2, (unsigned int)v);

    // ---- repeat 事件 0x29：角色日（job 匹配）----
    CEventBase* dayEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x29);
    if (dayEvent != 0 && dayEvent->IsEventing((CUser*)0))
    {
        if (get_charac_job() ==
            static_cast<CCharacterDayEvent*>(dayEvent)->GetEventJob())
        {
            _processCharacterDayEvent(prevFatigue, (unsigned int)v);
        }
    }
    // ---- repeat 事件 0x2b：疲劳 buff ----
    CEventBase* buffEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x2b);
    if (buffEvent != 0 && buffEvent->IsEventing((CUser*)0))
    {
        _processFatigueBuffEvent(prevFatigue, (unsigned int)v);
    }

    ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurability(this, v);
    m_postChecksumData.m_totalFatigueUsed =
        (short)(m_postChecksumData.m_totalFatigueUsed + (short)v);   // +0x79642
    if (get_charac_level() != 0)
    {
        int weight =
            G_CDataManager()->m_guildParameter.getGuildExpWeight(get_charac_level());
        add_guild_temp_exp((weight * v) / 100);
    }
    decCurCharacFatigueGrownUpBuff((unsigned int)v);
    if (static_cast<unsigned char>(m_guildDB.m_fundLevel) + 1 > 0x10)  // +0x8d03b = m_guildDB.m_fundLevel(+0x1b)
        reset_guild_temp_exp();
    processFatigueCharac(v);

    // ---- premium 无限疲劳：直接返回总疲劳 ----
    if (m_premium.isAffectedUnlimitFatigue())
    {
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 1：幸运点 * v ----
    CEventBase* ev1 = GlobalData::s_event_manager->GetRepeatEvent(1);
    // ORIG 0x0865619f：GetRepeatEvent(1) 后直接 IsEventing，无判空。
    if (ev1->IsEventing((CUser*)0))
    {
        IncCurCharacLuckPoint(
            // ORIG 0x086561bc `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
            G_CDataManager()->m_serverParameter.m_7c * v);
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 10：非活跃时走常规上限/燃烧/premium 结算 ----
    CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(10);
    // ORIG 0x086561ff：GetRepeatEvent(10) 后直接 IsEventing，无判空。
    if (!ev10->IsEventing(this))
    {
        if ((short)getCurCharacFatigue() == -1)
        {
            getCurCharacFatigue();
            return;
        }
        if (get_charac_memberkey() != 0 && is_connect_upper_member())
            add_member_pay_tex_fatigue_to_upper(v);
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->process_add_fatigue_count(this, v);

        unsigned short refill = 0;
        CEventBase* burn =
            GlobalData::s_event_manager->GetRepeatEvent(0xf);
        // ORIG 0x086562d7：GetRepeatEvent(0xf) 后直接 IsEventing，无判空。
        if (burn->IsEventing((CUser*)0))
        {
            refill = static_cast<CBurningFatigueEvent*>(burn)->getRefillFatigue(
                getCurCharacUsedFatigue(), (unsigned short)v);
        }
        if (IsHavePremiumAdvantage())
        {
            unsigned short prem = getCurCharacPremiumFatigue();
            unsigned int maxPrem = getCurCharacMaxPremiumFatigue() & 0xffff;
            if ((int)((unsigned int)prem + v) <= (int)maxPrem)
            {
                setCurCharacPremiumFatigue((unsigned short)(prem + v));
                if (refill != 0)
                {
                    gainCurCharacFatigue((unsigned int)refill);
                    SendFatigue();
                }
                IncCurCharacLuckPoint(
                    // ORIG 0x086563ae `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
                    G_CDataManager()->m_serverParameter.m_7c * v);
                getCurCharacTotalFatigue();
                return;
            }
            if (getCurCharacPremiumFatigue() <
                (unsigned short)getCurCharacMaxPremiumFatigue())
            {
                setCurCharacPremiumFatigue(
                    (unsigned short)getCurCharacMaxPremiumFatigue());
                v = ((unsigned int)prem + v) -
                    (getCurCharacMaxPremiumFatigue() & 0xffff);
            }
        }
        // ORIG 0x086565b0（P48 no-CSE）：getCurCharacMaxFatigue 在条件与赋值处
        // 各自重取，不缓存 maxFatigue 局部。
        unsigned int curFatigue = (unsigned int)getCurCharacFatigue() & 0xffff;
        int newFatigue;
        if ((int)(getCurCharacMaxFatigue() & 0xffff) < (int)(curFatigue + v))
            newFatigue = (int)(getCurCharacMaxFatigue() & 0xffff);
        else
            newFatigue = (int)(curFatigue + v);
        setCurCharacFatigue(newFatigue);
        IncCurCharacLuckPoint(
            // ORIG 0x086564a2 `mov 0xe0(%eax)`（4B int；SPS +0x78=m_78）
            (newFatigue - (int)curFatigue) * G_CDataManager()->m_serverParameter.m_78);
        if (refill != 0)
        {
            gainCurCharacFatigue((unsigned int)refill);
            SendFatigue();
        }
        getCurCharacFatigue();
        return;
    }
    getCurCharacTotalFatigue();
