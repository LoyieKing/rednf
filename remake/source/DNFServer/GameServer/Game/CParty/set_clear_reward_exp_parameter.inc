// ============================================================================
// CParty::set_clear_reward_exp_parameter —— ORIG T 0x085adb00（0x5d4 字节）
//   报告：docs/class_func_reports/CParty/set_clear_reward_exp_parameter.md
//   逐段对照 ## 汇编 + ## 反编译 C 转写。要点：
//   - PC 房间燃烧经验：GetExpFactor()/100 为【整数除法】（imul 0x51eb851f/sar 5）；
//     双倍经验 GetExpFactor()/100.0f 为浮点除（flds _DAT_08cbbbcc=100.0f）。
//   - m_pcRoomSameUserBonus(+0x34) 按原始 dword 位型写入（读取方按 float 解释）。
//   - 回归用户队伍奖励 0x3e4ccccd = 0.2f。
//   - 成长之力：get_growth_power_exp_reward 返回 dword，取 (v>>16)&0xff / 100。
//   - getAddExpRateFromPremium(0x4f)：ORIG 直呼 CUser 方法（W 0x085bfd76 =
//     WongWork::CUserPremium::GetAddExpBonus(type)：type∈[0,0x69] 时返回
//     g_SPremiumInfo[type].m_field44）；该转发链未建模为成员函数，
//     就地取权威表字段（与 sumGainedEachExp 同惯例）。
// ============================================================================
// [SweepCParty] 补真缺口：GetChannelScript 前第三次独立 call G_CDataManager
//（旧码复用 dm ⇒ p167 MISS 1，已消）。p167 58=58 CLEAR（余 C1/C2 别名）。
void CParty::set_clear_reward_exp_parameter(CUser* user, uint exp,
                                            stClearRewardExpParameter_t& param)   // [G07] 拼写同 ORIG（见 CParty.h 注释）
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager（1 次，局部 dm 已存在但少一次重调）。@0x85adb00
    // [2026-09-11 CPartyWriter] 两处落地（读数 431→431，净零但方向与 ORIG 形一致、四数不劣）：
    // ①`int dungeonIndex` 具名槽删除、结果直传 find_dungeon 实参（ORIG 085adb1b..085adb2a 无槽）；
    // ②`*reinterpret_cast<int*>(&param.m_pcRoomSameUserBonus)=` 改 float 位型直赋，复现 ORIG
    // 085adf20 `mov %edx,0x34(%eax)` 单条 disp（旧码 `add $0x34,%eax; mov %edx,(%eax)` 两步，P238）。
    // 残余 416/431：ORIG 085ae0xx 段 float 死局部两段初始化（`mov $0x0,%eax; mov %eax,-0x28`）
    // 与 0x44/0x48 段写入次序（我方 p 段块序滑动）属 N82/N88 族，未再烧轮次。
    CDataManager* dm = G_CDataManager();                    // -0x38 local_3c
    // [2026-09-11 P0] ORIG 085adb1b..085adb2a：get_dungeon_index 结果**无槽**，直进
    // `0x4(%esp)` 作 find_dungeon 实参（旧码 `int dungeonIndex` 多 1 store+1 load）。
    CDungeon* dungeon = dm->find_dungeon(
        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());  // -0x34
    ServerParameterScript* spScript = &dm->m_serverParameter;  // -0x30 local_34（+0x68）

    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x294a, "Unknown Dungeon %d",
            reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }
    if (user == 0)
    {
        return;
    }

    // +0x096..+0x119：四项基础加成
    float avatarBonus = CalcAvatarBonus();                  // -0x2c local_30

    float pcRoomBurningRate = 0.0f;                         // -0x28 local_2c
    {
        // [G7-sub] P0 槽律：ORIG 0x085adbbe `call GetRepeatEvent(8)` 结果**无栈槽**
        // （直进 %eax → `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%ecx; call *%ecx`）
        // ⇒ 原码无 ev8/burning 具名指针；我方原 -0x1c/-0x18 两槽已删（内联链）。
        if (static_cast<CPCRoomBurningEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(8))->IsEventing(user))
        {
            // ORIG 0x085adbea 第二次调用结果亦无槽（直进 (%esp) 调 GetExpFactor）。
            pcRoomBurningRate = static_cast<float>(
                static_cast<CPCRoomBurningEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(8))
                    ->GetExpFactor() / 100);
        }
    }

    float expDoubleRate = 0.0f;                             // -0x24 local_28
    {
        // [G7-sub] P0 槽律：ORIG 0x085adc31 GetRepeatEvent(3) 无槽（同形虚调用）；
        // 0x085adc5e 第二次调用结果亦无槽 ⇒ 删 ev3/doubleEv 具名（我方 -0x10 等槽）。
        if (static_cast<CExpDoubleEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(3))
                ->IsEventing(static_cast<CUser*>(0)))
        {
            expDoubleRate = static_cast<float>(
                static_cast<CExpDoubleEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(3))
                    ->GetExpFactor()) / 100.0f;
        }
    }

    float periodItemRate = 0.0f;                            // -0x20 local_24
    // [R15a11 P360/P225] ORIG 0x085adc8f/0x085adc9e 两处 `test %al,%al; je 0x085adcb0`
    //   **同落出口**（无 bool 汇聚岛、无 `mov $1/$0` 物化）；写 `&&` 链时 4.4 恒插
    //   `mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`（+5 条）⇒ 拆嵌套 if 复现。
    if (user->IsExpItemAffected())
    {
        if (user->EnablePeriodExpItem())
        {
            periodItemRate = user->GetExpAffectRate();
        }
    }

    int pcRoomSameUserBonus = 0;                            // -0x1c local_20
    // ORIG 0x085adcbe/0x085adcd2：GetPCRoomNo 两次独立调用（P48），G_CDataManager 二次直呼
    // [R6a10 2026-09-12 P360 形态对齐] ORIG 0x085adcb8..0x085adce1：
    // `call GetPCRoomNo; test %eax,%eax; setne %al; test %al,%al; je 0x085adcf8` 与
    // 第二条件 `test %al,%al; je 0x085adcf8` **同落出口**（短路与语义不变）。
    // 写成 `&&` 链会折掉 setne；拆嵌套 if 即复现（探针 /tmp/verify357/R6a10/probe.cpp t2）。
    if (static_cast<unsigned int>(user->GetPCRoomNo()) != 0)
    {
        if (existSamePCRoomUser(static_cast<unsigned int>(user->GetPCRoomNo())))
        {
            // [R4a17 2026-09-12 SEMANTIC-DIFF 修复] ORIG 0x085adcea..0x085adcf5 段
            // `call G_CDataManager; mov 0x7d4(%eax),%eax; mov %eax,-0x1c(%ebp)` =
            // **原始 dword 位型直搬**（无 x87 转换）；旧码隐式 float→int 截断
            // （flds+fldcw+fistpl）后按位型写入 +0x34 ⇒ m_pcRoomSameUserBonus 浮点值错误。
            pcRoomSameUserBonus = *reinterpret_cast<int*>(
                &G_CDataManager()->m_serverParameter.m_76c);  // +0x7d4 原始 dword
        }
    }

    float returnUserBonus = 0.0f;                           // -0x18 local_1c
    if (IsReturnUserParty())
    {
        returnUserBonus = 0.2f;                             // ORIG 0x3e4ccccd
    }

    // +0x21a..+0x27c：param 基础字段回填
    param.m_deadPenaltyRate = calDeadUserDungeonPenalty(param.m_deadMemberCount);
    param.m_rankBonusWeight = spScript->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(param.m_rankBonusIdx));
    param.m_avatarBonus = avatarBonus;
    param.m_creatureBonus = CalcCreatureBonus(user);
    param.m_pcRoomBurningRate = pcRoomBurningRate;
    param.m_expDoubleRate = expDoubleRate;

    // +0x27f..+0x2d5：燃烧疲劳奖励
    CBurningFatigueEvent* fatigueEvent = static_cast<CBurningFatigueEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigueEvent->IsEventing(static_cast<CUser*>(0)))
    {
        param.m_burningFatigueBonusExp =
            fatigueEvent->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    // +0x2d8..+0x394：频道加成（服务器类型 1/2 走 getBonusRateEventServer）
    // ORIG 0x085addd3 第三次独立 call G_CDataManager（旧码复用 dm ⇒ p167 MISS 1）。
    channel_script_t* channelScript =
        static_cast<channel_script_t*>(G_CDataManager()->GetChannelScript());
    // ORIG 0x085addf3..0x085ae094：G_CEnvironment 各判定点独立重读（P48），bool 物化
    bool eventServer =
        (G_CEnvironment()->m_serverEnvir.m_serverType == 1) ||
        (G_CEnvironment()->m_serverEnvir.m_serverType == 2);
    if (eventServer)
    {
        param.m_serverChannelRate = channelScript->getBonusRateEventServer(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            static_cast<unsigned int>(G_CEnvironment()->get_channel_no()));
    }
    else
    {
        param.m_serverChannelRate = channelScript->getBonusRate(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            static_cast<unsigned int>(G_CEnvironment()->get_channel_no()),
            static_cast<unsigned int>(dungeon->get_index()));
    }

    // +0x397..+0x3cd：会员/服务器/全局效果加成
    if (user->IsHavePremiumAdvantage())
    {
        param.m_premiumAdvantageRate =
            static_cast<float>(user->GetPremiumInfo()->GetAdvantageExpRate()) /
            100.0f;
    }
    param.m_serverEventExpRate =
        static_cast<float>(CServerEvent::GetExpRate()) / 100.0f;
    if (GlobalData::s_globalEffectManager_->isAffectedEffect(2))
    {
        param.m_globalEffectRate =
            GlobalData::s_globalEffectManager_->getEffectValue(2);
    }

    // +0x41a..+0x429：网吧同号/回归用户奖励（dword 位型写入）
    // [2026-09-11 P238] ORIG 085adf20 `mov %edx,0x34(%eax)` 单条 disp 写（edx 中转）；
    // 旧码 `*reinterpret_cast<int*>(&param.m_pcRoomSameUserBonus)=` 产 `add $0x34,%eax;
    // mov %edx,(%eax)` 取址两步。改 float 位型直赋（4.4 对 float 纯搬恒 mov 直搬）。
    param.m_pcRoomSameUserBonus = *reinterpret_cast<float*>(&pcRoomSameUserBonus);
    param.m_returnUserPartyBonus = returnUserBonus;

    // +0x42c..+0x474：事件角色队伍经验比例
    if (!IsEventCharacParty())
    {
        param.m_eventCharacExpRatio = 0.0f;
    }
    else
    {
        EventNewCharacterReward* newCharEv = reinterpret_cast<EventNewCharacterReward*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xa2));
        if (newCharEv != 0)
        {
            param.m_eventCharacExpRatio = newCharEv->eventExpRatio();
        }
    }

    // +0x477..+0x498：管理加成
    param.m_manageBonus =
        CalcurateManageBonus(static_cast<ManageBonusType::T>(0), user, exp);

    // +0x49e..+0x4dd：成长之力（STExpReward 局部回填 dword0 后取字节 2）
    STExpReward growthTmp;                                     // -0x3c
    growthTmp.m_reward =
        user->get_growth_power_exp_reward().m_reward;          // sret dword → +0x00
    param.m_growthPowerExpRate =
        static_cast<float>((growthTmp.m_reward >> 16) & 0xff) / 100.0f;

    // +0x4e0..+0x530：会员额外经验（premium type 0x4f）
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
    {
        param.m_premiumAddExpRate = 0.0f;
    }
    else
    {
        // ORIG 0x085ae005：直呼 CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE)（W 0x085bfd76）
        param.m_premiumAddExpRate =
            static_cast<float>(static_cast<unsigned int>(
                user->getAddExpRateFromPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))) /
            100.0f;
    }

    // +0x533..+0x57b：周期经验道具（与重叠加成事件互斥清零）
    param.m_periodExpItemRate = periodItemRate;
    if (param.m_periodExpItemRate != 0.0f &&
        GlobalData::s_event_manager->IsExistOverlappedExpBonusEvent())
    {
        param.m_periodExpItemRate = 0.0f;
    }

    // +0x57b..+0x5c7：清零四项角色 bonus exp
    user->set_charac_party_bonus_exp(0);
    user->set_charac_member_bonus_exp(0);
    user->set_charac_fatigue_buf_bonus_exp(0);
    user->set_charac_seria_buf_bonus_exp(0);
}