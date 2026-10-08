// ============================================================================
// generateRewardExp（ORIG 0x08149e20）
// [b28d-46] baseExp 具名 float（P252 复现 fstps -0xf0 槽往返）、tower 内联表达式。
// [L5Free49to60 2026-09-11] 244 vs 267 → **244 vs 245**（gap −23→−1，bcmp NDIFF
// 139→78）。本波旋钮（全为 P0/P183 删多余具名局部，依据 ORIG 无槽往返实证）：
// ① 删 ev8/ev3/evF，GetRepeatEvent(8/3/0xf)->IsEventing() 内联（ORIG 0x08149fa7..
//    0x08149fb8 虚调用链全程 %edx/%ecx 寄存器直用，无槽）；
// ② 删 dm2/chScript/env/channelNo/channelType 五局部，getBonusRate 单表达式链
//    （ORIG 0x08149f4c get_index→%edi、0x08149f5b GetChannelScript→%esi、
//    0x08149f62/68 G_CEnvironment+0x378→%ebx、0x08149f73 channel_no→%eax）；
// ③ pcrBurning/expDouble/premiumRate 去 `= 0.0f`（ORIG 三槽 c4/bc/b8/a8 无清零，
//    仅条件分支内 fstps 写入 ⇒ P131/P171）；
// ④ 删 pre（GetPremiumInfo），内联链式（ORIG 0x0814a0e1/0x0814a0e6 直用 %eax）；
// ⑤ `m_rewardExpWeight[1 + a]` → `[a]`：ORIG `flds 0x4(%eax,%edx,4)` 由 stDeathTower_t
//    的 m_apcExpWeight@+0x0 提供 +0x4 基址，旧写法多发 `add $0x1,%edx`。
// 残差（封顶）：帧 0x13c vs 0x15c 全帧槽号平移 0x20 —— ORIG 把 8 个 float 局部排在
// param(-0xcc,8B) 之后的 -0xc4..-0xa4、baseExp 落 -0xf0，4.4 恒「聚合对象优先占低槽、
// 标量分区排布」（P332/N82附）；尾段 m_rewardExp[memberIdx] ORIG `lea 0x4(%edx),%ecx`
// vs 我方 `add`（P30/N4 结构性伪影）；日志段 `.o` 重定位 imm 伪差。语义闭合。
// ============================================================================
bool CBossPlay::generateRewardExp(int memberIdx, unsigned int a, unsigned int b,
                                 const CDungeon* dungeon)
{
    CUser* user = m_party->get_user(memberIdx);
    if (user == 0)
        return 0;

    // 基础经验 = round((float)(a*b) × 死亡之塔费率)。GetTowerScript 返回结构体基址，
    // ORIG flds 0x4(%eax,%edx,4) 读 m_rewardExpWeight[a]（stDeathTower_t 里
    // m_apcExpWeight@+0x0 之后的表，索引 a 直接进 scaled 寻址 ⇒ 无 add $1）；
    // x87 fistpll 就近舍入，直接 (long long) 强转等价。
    // [L5Free49to60] 原写法 `[1 + a]` 多发 `add $0x1,%edx` 且破坏 disp 折叠。
    unsigned int clearExp;
    {
        // [b28d-46 P252 实测] ORIG 先 fildll(a*b)→fstps 具名 float 槽 -0xf0，再取
        // tower 并 fmuls mem；具名 float 局部复现该槽往返。
        float baseExp = (float)(long long)((unsigned int)(a * b));
        // ORIG tower 无槽往返（GetTowerScript 结果直用 %eax）⇒ 内联表达式（P0/P183）。
        clearExp = (unsigned int)(long long)(
            baseExp * G_CDataManager()->GetTowerScript(
                dungeon->get_index())->m_rewardExpWeight[a]);
    }

    stClearRewardBonusExp_t bonus;
    stClearRewardExpParameter_t param;
    // [DNF-FIX 2026-10-08 c30] ORIG 0x08149f01..0x0814a102：各加成系数直接写入 param（param@-0xcc：
    //   +0x08/-0xc4 avatar、+0x0c/-0xc0 creature、+0x28/-0xa4 serverEvent、+0x20/-0xac channel、
    //   +0x10/-0xbc pcRoomBurning、+0x14/-0xb8 expDouble、+0x1c/-0xb0 burningFatigue、
    //   +0x24/-0xa8 premiumAdvantage），随后 0x0814a130 getClearRewardBonusExp(user, exp,
    //   param, bonus) 读取；旧码用独立局部，param 恒为 ctor 零值 ⇒ 塔通关经验加成全丢。
    param.m_avatarBonus = (float)m_party->getAvatarBonus();
    param.m_creatureBonus = (float)m_party->getCreatureBonus(user);

    param.m_serverEventExpRate = (float)CServerEvent::GetExpRate() / 100.0f;

    param.m_serverChannelRate = ((channel_script_t*)G_CDataManager()->GetChannelScript())->getBonusRate(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        (unsigned int)G_CEnvironment()->get_channel_no(),
        dungeon->get_index());

    if (GlobalData::s_event_manager->GetRepeatEvent(8)->IsEventing(user))
        // ORIG：整型 /100（imul 0x51eb851f 形态）后才转 float
        param.m_pcRoomBurningRate = (float)(((CPCRoomBurningEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    if (GlobalData::s_event_manager->GetRepeatEvent(3)->IsEventing(0))
        param.m_expDoubleRate = (float)((CExpDoubleEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f;

    if (GlobalData::s_event_manager->GetRepeatEvent(0xf)->IsEventing(0))
        param.m_burningFatigueBonusExp =
            ((CBurningFatigueEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(0xf))->getBonusExpRate(user->getCurCharacUsedFatigue());

    if (user->IsHavePremiumAdvantage())
    {
        param.m_premiumAdvantageRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;
    }

    unsigned int total = m_party->getClearRewardBonusExp(user, clearExp, param, bonus);

    if (0x10c8e0 < total)
    {
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x159, "BOSS_LOG : EROOR BOSSTOWER CLEAR REWARD EXP(%u)\n", total);
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x163, "BOSS_LOG : BOSSTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)param.m_avatarBonus, (double)param.m_creatureBonus,
            (double)param.m_pcRoomBurningRate, (double)param.m_expDoubleRate,
            (double)param.m_serverChannelRate, (double)param.m_premiumAdvantageRate,
            (double)param.m_serverEventExpRate, param.m_burningFatigueBonusExp);
        total = 0x10c8e0;
    }
    m_rewardExp[memberIdx] = (int)total;
    return 1;
}
