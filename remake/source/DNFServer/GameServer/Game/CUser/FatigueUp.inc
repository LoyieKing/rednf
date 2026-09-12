// §6.2 巨件核销：P167 100/100 [0缺0多]；gap 616→634（our+18）；残差归因 N82
// 槽号错位 + esm 访问器重物化族（P112），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
// b28d-solo-CUser p167 复核（ORIG 124 / OUR 122）：已修 4 处 put_binary 重载
// （char*→const char*，2×expansion + 2×init4，P167 经典重载选错）。
// 余 6 缺口待后续：MISSING getCurCharacInvenR(const)×2 / GetAvatarItemMgrR(const)×2 /
//   MonsterKillItem::C1×1 / GetInvenRef(int,int) const×1；
//   EXTRA set_add_info(int)×2 / GetInvenRef 非 const×1 / getItemList()×1。
// （访问器 cv/重载选择差异，非行为分叉证据；形态残差归 N82/P112。）
    // 0x08655c60 主分支（FatigueUp.md）：v==0→1；BreakAway 幸运点先结算；
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08655c60 | _ZN5CUser9FatigueUpEi
// [b28d-GrpD 判词·结构化 N] ORIG 0x08655c60：残差为具名临时槽化（帧 0x64 vs ORIG
//   0x54）引起的整段 mov 重排；setcc（setne/sete）与 je/jne 落点体互换均为等价
//   （判据⑦）；字段偏移（+0x8e064/+0x8e08c/0x9c/0x32/0x64）与事件脚本 id（0x19b 等）
//   逐条对齐。
// [R20a09 更正，2026-09-13] 上述「无语义缺口」已被推翻一处：角色日事件 job 判等
//   （ORIG 0x0865600f 段）为 **16→32 有符号加宽**（`cwtl`），我方原为 `movzwl`
//   零扩展 ⇒ `job==-1 && eventJob==0xffff` 时判等结果分叉（ORIG 成立）。已按
//   `(short)` 源形修复（详见本文件 118-124 行注释与 R20a09 报告）。
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
            // [L9-C组J PATCH J4] ORIG 0x08655d3c 段 `mov 0x8e064(%eax),%eax; mov %eax,%edx;
            //   add 0xc(%ebp),%edx; mov %edx,0x8e064(%eax)` = 成员值读改写（disp 形）；
            //   裸指针 cast 在 4.4 产 lea 取址形（多 1 条）⇒ 具型视图成员（POD，无符号发射）。
            struct ChaosFatigueTail
            {
                char m_pad[0x8e064];
                int m_chaosFatigue;   // +0x8e064
            };
            reinterpret_cast<ChaosFatigueTail*>(this)->m_chaosFatigue += v;
        }
    }
    m_fatigueGain = v;                             // +0x8e08c
    unsigned short prevFatigue = (unsigned short)getCurCharacUsedFatigue();
    incCurCharacUsedFatigue((unsigned short)v);
    unsigned short nowFatigue = (unsigned short)getCurCharacUsedFatigue();
    // 2026-09-11 CuA A组（语义轴，ORIG 直证）：ORIG 0x08655dd0-0x08655e02 实序列为
    //   `shr %ax; movzwl; imul $0x147b; shr $0x10; mov %eax,%edx; shr %dx; mov %edx,%eax;
    //    shl $2; add %edx; lea 0x0(,%eax,4),%edx; add %edx; add %eax,%eax; mov %ecx,%edx;
    //    sub %ax,%dx; test %dx,%dx; sete` ⇒ 判定式 = **fat - 50*((fat/25)/2) == 0**，
    //   即 `fat % 50 == 0`（尾 `add %eax,%eax` 把 25x 变 50x）。我方旧形乘数写 -0x2a(42)
    //   （= fat - 42*floor(fat/50)）⇒ fat=50k 时漏报、fat=42 时误报；改回 -0x32。
    // 2026-09-11 R5 补（imm/bits 命中终判）：本段 ORIG `imul $0x147b`（5243 ≈ 2^16·2/25）
    //   vs 我方 `imul $0x47af`（18351 ≈ 2^16·7/25）。5 形探针实测（/tmp/r5/probe/p_div.cpp
    //   t1 `(f>>1&0x7fff)/0x19`｜t2 `(f>>1)/0x19`｜t3 具名 `unsigned short half` + `/0x19`
    //   ｜t4 `int half` / 25｜t5 `u16 q = half/25`）**全部**产 `imul $0x47af`（或 `imul %edx`），
    //   无一产 0x147b ⇒ ORIG 的简化 16 位除法 lowering（4.1）在 4.4 下不可造（N85 同族）
    //   ⇒ 判 N，勿再试写法。
    if ((short)((short)nowFatigue + (short)((nowFatigue >> 1 & 0x7fff) / 0x19) * -0x32) == 0)
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
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->inc_user_fatigue(this, (unsigned int)v);
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->send_gift_fatigue_item(this, (unsigned int)v);
    if (static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()
            ->get_event_script_mng())
            ->is_eventing(0x19b))
    {
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->send_bring_up_item_fatigue(this, v);
    }

    if (isJoinedSchoolPointEvent())
        calcCurCharacSchoolPoint(prevFatigue);

    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, (HeroMissionCondition::MissionNo::T)2, (unsigned int)v);

    // ---- repeat 事件 0x29：角色日（job 匹配）----
    CEventBase* dayEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x29);
    if (dayEvent != 0)
    {
        if (dayEvent->IsEventing((CUser*)0))
        {
            // [R20a09 ④] ORIG 0x08656014 为 `cwtl`（**有符号** 16→32 加宽）：
            // `call GetEventJob`（返回 unsigned short，ctor 置 -1 ⇒ 0xffff）后按
            // short 解读 ⇒ job==-1(0xff) 且 eventJob==0xffff 时判等成立并触发
            // _processCharacterDayEvent；写成无转换（零扩展 65535）则不成立 ⇒ 分叉。
            if (get_charac_job() ==
                (short)static_cast<CCharacterDayEvent*>(dayEvent)->GetEventJob())
            {
                _processCharacterDayEvent(prevFatigue, (unsigned int)v);
            }
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
}