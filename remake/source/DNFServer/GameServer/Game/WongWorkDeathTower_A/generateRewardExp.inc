// ==============================================================================
// CPlayData::generateRewardExp（ORIG T 0x08462538）
//
// 权重表：ORIG `flds 0xc(dm + (stage+tier)*4)` = m_deathTower.m_rewardExpWeight
// （tier 0x1464）/ m_illusionRewardExpWeight（tier 0x15f4）——基址
// 0x5198+0x4/0x644 与 0xc+0x1464*4/0xc+0x15f4*4 精确吻合（float 读取，ORIG
// 报告 WongWork__CDeathTower__CPlayData/generateRewardExp.md）；所得 exp 与最终
// 结算结果 >0x10c8e0(1100000) 时写 BLOOD_LOG 并截断。各加成系数仅进日志参数流。
// ==============================================================================
// [b28d-24 历史] DIFF 残148（P282 候选）: vcall 偏移 52 ORIG 3 次 vs 我方 2 次。
// [2026-09-10 round5 复核] **p167 CALLSITE CLEAN 36/36**：残余仅 C1/C2 变体差
//   （_ZN23stClearRewardBonusExp_t C1==C2 同址 0x0814a800、_ZN27stClearRewardExpParameter_t
//   C1==C2 同址 0x0814a8b8 ⇒ P93 伪差，非缺口）；vcall +0x34（IsEventing）三处已齐。
//   本轮修：删 channelNo/dungeonIdx 具名局部（P0，值经 %ebx/%esi 跨 call 保活直传）、
//   G_CEnvironment 两处调用改复用 env。残余 = 我方 308 vs ORIG 291（帧 0x178 vs 0x130：
//   具名 float 局部槽基差，N82/N95 族；mn_cmp 残差全为「+mov|+mov」槽往返对）⇒ N 封顶。
// [M1S3EmblemLink 2026-09-11] **推翻「复用 env」旧修**（p167 复测 MISSING
//   `_Z14G_CEnvironmentv` -1；Ghidra ORIG 亦两次 `G_CEnvironment()`：一次供
//   `get_channel_no`、一次供 `*(env+0x378)`）⇒ 恢复两处直调，p167 归零 36/36（仅
//   C1/C2 别名对）。另落 2 处 P0：① 删 `limitFlag` 具名槽（ORIG `call
//   limitOfStackableItemInTower; test %al,%al; je`）② 删 `logWeight` 具名槽
//   （ORIG 0x084626xx `flds 0xc(%eax,%edx,4)` 直取后 `fstpl 0x20(%esp)`）。
//   读数 308→305（gap 14，ORIG 291）。试删 `expInOut`（照 Ghidra `&local_20`）
//   ⇒ 305→306 反劣，已回滚（负结论见函数体）。
//   残余：帧 0x160 vs ORIG 0x130（我方对象/float 槽分配差，N82 族；ORIG 对象
//   bonus 在 -0x68/clearParam -0xc8、floats -0x8c..-0xc0，我方补偿性换位）⇒ 封顶。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N3）→ 试=①事件 0xf 块具名 ev 核对：
//   ORIG 0x0846279f..0x084627a7 `mov %eax,-0x14(%ebp); mov -0x14(%ebp),%eax` **有**具名槽
//   ⇒ ev 非多余（与 0x08462701/0x08462775 两处已内联块不同，勿混）②槽集合比对：
//   ORIG 29 槽 / OURS 29 槽但排布互置（user/reward ORIG -0x18/-0x1c vs OURS -0x3c/-0x38；
//   float 组 ORIG -0x9c..-0xbc vs OURS -0x14..-0x34；帧实测 0x130 vs 0x150）⇒ 维持 N3/N82。
// ORIG 0x08462538
void CDeathTower::CPlayData::generateRewardExp(int memberIdx, int stage,
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    // [M1S3EmblemLink 2026-09-11 P0] ORIG 无 limitFlag 具名槽（`call
    //   limitOfStackableItemInTower; test %al,%al; je`，0x084625xx）⇒ 内联。
    if (dungeonMgr.getDungeon()->limitOfStackableItemInTower() != '\0')
    {
        // ORIG 两分支重复代码：32 位乘积先物化、G_CDataManager 后取（求值序照 ORIG）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_rewardExpWeight[stage]);
    }
    else
    {
        // ORIG 非有限堆叠档（+0x15f4）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_illusionRewardExpWeight[stage]);
    }

    if (1100000 < reward)
    {
        // [M1S3EmblemLink 2026-09-11 P0] ORIG 无 logWeight 具名槽：0x084626xx 处
        //   `flds 0xc(%eax,%edx,4)` 直接内存取（G_CDataManager 后取），其前仅
        //   `mov -0x1c(%ebp),%eax`（reward）⇒ 权重表达式内联进实参。
        LogManager::logFormat(1, "DeathTower.cpp", __PRETTY_FUNCTION__, 0x2ff,
            "BLOOD_LOG : EROOR DEATHTOWER REWARD EXP(%u), LAST STAGE(%d), BASE EXP(%u), EXP WEIGHT(%f)\n",
            reward, stage, baseExp,
            (double)G_CDataManager()->m_deathTower.m_rewardExpWeight[stage]);
    }

    stClearRewardBonusExp_t bonus;            // ORIG ctor 0x0814a800 @-0x64
    stClearRewardExpParameter_t clearParam;   // ORIG ctor 0x0814a8b8 @-0xc4
    // [DNF-FIX 2026-10-08 c30] ORIG 0x084626e3..0x0846290d：各系数直接写入 clearParam（@-0xc4：
    //   +0x08/-0xbc avatar、+0x0c/-0xb8 creature、+0x10/-0xb4 pcRoomBurning、+0x14/-0xb0
    //   expDouble、+0x1c/-0xa8 burningFatigue、+0x20/-0xa4 channel、+0x24/-0xa0 premium、
    //   +0x28/-0x9c serverEvent、+0x3c/-0x88 manageBonus），0x08462938
    //   getClearRewardBonusExp(user, exp, clearParam, bonus) 读取；旧码独立局部 ⇒ 加成全丢。
    clearParam.m_avatarBonus = (float)party->getAvatarBonus();
    clearParam.m_creatureBonus = (float)party->getCreatureBonus(user);

    // 事件 8：PC 房间燃烧（整除 /100 —— imul 0x51eb851f 形态实证）
    if (GlobalData::s_event_manager->GetRepeatEvent(8)->IsEventing(user))
        clearParam.m_pcRoomBurningRate = (float)(((CPCRoomBurningEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    // 事件 3：双倍经验（浮点 /100.0f）
    if (GlobalData::s_event_manager->GetRepeatEvent(3)->IsEventing(0))
        clearParam.m_expDoubleRate = (float)((CExpDoubleEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f;

    {   // 事件 0xf：疲劳燃烧加成（千分率原始值；ORIG -0x14 具名 ev 槽）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if (ev->IsEventing(0))
            clearParam.m_burningFatigueBonusExp =
                ((CBurningFatigueEvent*)ev)->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    clearParam.m_serverChannelRate =
        channelScript->getBonusRate(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            (unsigned int)G_CEnvironment()->get_channel_no(),
            (unsigned int)dungeonMgr.getDungeon()->get_index());

    if (user->IsHavePremiumAdvantage())
        clearParam.m_premiumAdvantageRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;

    clearParam.m_serverEventExpRate = (float)CServerEvent::GetExpRate() / 100.0f;

    clearParam.m_manageBonus =
        party->CalcurateManageBonus((ManageBonusType::T)0, user, reward);

    unsigned int expInOut = reward;
    reward = party->getClearRewardBonusExp(user, expInOut, clearParam, bonus);

    if (1100000 < reward)
    {
        LogManager::logFormat(1, "DeathTower.cpp", __PRETTY_FUNCTION__, 0x324,
            "BLOOD_LOG : EROOR DEATHTOWER CLEAR REWARD EXP(%u)\n", reward);
        LogManager::logFormat(1, "DeathTower.cpp", __PRETTY_FUNCTION__, 0x32e,
            "BLOOD_LOG : DEATHTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)clearParam.m_avatarBonus, (double)clearParam.m_creatureBonus,
            (double)clearParam.m_pcRoomBurningRate, (double)clearParam.m_expDoubleRate,
            (double)clearParam.m_serverChannelRate, (double)clearParam.m_premiumAdvantageRate,
            (double)clearParam.m_serverEventExpRate,
            (int)clearParam.m_burningFatigueBonusExp);
        reward = 1100000;
    }

    m_rewardExp[memberIdx] = (int)reward;   // ORIG *(uint*)(this+(idx+0x26c)*4+8)
}