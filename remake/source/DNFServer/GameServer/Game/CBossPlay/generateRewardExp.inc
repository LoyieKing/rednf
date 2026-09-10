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
    float avatarBonus = (float)m_party->getAvatarBonus();
    float creatureBonus = (float)m_party->getCreatureBonus(user);

    float serverRate = (float)CServerEvent::GetExpRate() / kPercentDivisor;

    // [L5Free49to60 P0/P183] ORIG 对 dm/chScript/env/channelNo/channelType 全无槽往返：
    // get_index→%edi（08149f4c）、G_CDataManager+GetChannelScript→%esi（08149f5b）、
    // G_CEnvironment 读 +0x378→%ebx（08149f62/68）、G_CEnvironment+get_channel_no→%eax
    // （08149f73）后直接 call getBonusRate（08149f87）⇒ 原码为单表达式链，无具名局部。
    // 渠道组字节：内联读 +0x378（= Server_Envir m_serverGroup，+0x198+0x1e0），
    // 不 call get_server_group()。
    float channelRate = ((channel_script_t*)G_CDataManager()->GetChannelScript())->getBonusRate(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        (unsigned int)G_CEnvironment()->get_channel_no(),
        dungeon->get_index());

    // [L5Free49to60 P131/P171] ORIG 对 pcrBurning(-0xbc)/expDouble(-0xb8)/premiumRate(-0xa8)
    // 三个 float 槽**无**清零指令（仅条件分支内 fstps 写入）⇒ 原码无 `= 0.0f`
    // 初始化器；`=0.0f` 每处多发 `mov $0,%eax; mov %eax,slot` 两条。
    float pcrBurning;
    float expDouble;
    int burningRate;
    float premiumRate;

    // [L5Free49to60 P0/P183] ORIG 对 GetRepeatEvent(8/3/0xf) 结果**无槽往返**
    //（`call GetRepeatEvent; mov (%eax),%edx; add $0x34; mov (%edx),%ecx; …; call *%ecx`）
    // ⇒ 原码把 receiver 写成内联表达式，无 ev8/ev3/evF 具名指针。
    if (GlobalData::s_event_manager->GetRepeatEvent(8)->IsEventing(user))
        // ORIG：整型 /100（imul 0x51eb851f 形态）后才转 float
        pcrBurning = (float)(((CPCRoomBurningEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    if (GlobalData::s_event_manager->GetRepeatEvent(3)->IsEventing(0))
        expDouble = (float)((CExpDoubleEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / kPercentDivisor;

    if (GlobalData::s_event_manager->GetRepeatEvent(0xf)->IsEventing(0))
        burningRate =
            ((CBurningFatigueEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(0xf))->getBonusExpRate(user->getCurCharacUsedFatigue());

    if (user->IsHavePremiumAdvantage())
    {
        // [L5Free49to60 P0/P183] ORIG 0x0814a0e1/0x0814a0e6：GetPremiumInfo 结果
        // **直用 %eax** 压 esp（无槽往返）⇒ 原码内联链式，无 pre 具名指针。
        premiumRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / kPercentDivisor;
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
            (double)avatarBonus, (double)creatureBonus, (double)pcrBurning, (double)expDouble,
            (double)channelRate, (double)premiumRate, (double)serverRate, burningRate);
        total = 0x10c8e0;
    }
    m_rewardExp[memberIdx] = (int)total;
    return 1;
}
