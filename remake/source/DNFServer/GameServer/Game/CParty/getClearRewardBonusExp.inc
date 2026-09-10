unsigned int CParty::getClearRewardBonusExp(CUser* user, unsigned int& exp,
                                            stClearRewardExpParameter_t const& param,
                                            stClearRewardBonusExp_t& bonus)
{
    // b28d-49 判词：N 封顶（N85 x87 + N82 帧/槽）。P167 CLEAR。@0x85ad278
    unsigned int expPerUser = exp;   // 扣减前备份（日志 ExpPerUser）
    unsigned int effectExp = 0;      // globalEffect 择优值（日志第 8 参数）

    if (param.m_deadPenaltyRate != 0.0f)
        exp = (unsigned int)(long long)((double)(long long)exp *
                                        (1.0 - param.m_deadPenaltyRate));

    bonus.m_rankWeightExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_rankBonusWeight);
    bonus.m_avatarBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_avatarBonus);
    if (param.m_avatarBonus != 0.0f)
        bonus.m_avatarBonusExp = std::max(bonus.m_avatarBonusExp, 1u);
    bonus.m_creatureBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_creatureBonus);
    if (param.m_creatureBonus != 0.0f)
        bonus.m_creatureBonusExp = std::max(bonus.m_creatureBonusExp, 1u);
    bonus.m_periodItemExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_periodExpItemRate);
    bonus.m_pcRoomSameUserExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_pcRoomSameUserBonus);
    bonus.m_returnUserExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_returnUserPartyBonus);
    bonus.m_eventCharacExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_eventCharacExpRatio);

    // pcRoomBurning(2)/expDouble(1) 先到者优先；globalEffect 仅在类型 0/1/2
    // 且更大时覆盖为 4（类型 3 保留，ORIG +0x34c 分支）。
    if (param.m_pcRoomBurningRate != 0.0f)
    {
        bonus.m_selectedBonusExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_pcRoomBurningRate);
        bonus.m_bestType = 2;
    }
    else if (param.m_expDoubleRate != 0.0f)
    {
        bonus.m_selectedBonusExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_expDoubleRate);
        bonus.m_bestType = 1;
    }

    if (param.m_globalEffectRate != 0.0f)
    {
        effectExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_globalEffectRate);
        // P251/P279：ORIG 0x85ad6xx 为 `mov 0x8(%eax),%eax; cmp $0x1,%eax; jl;
        // cmp $0x2,%eax; jle; cmp $0x3,%eax; je; jmp` 顺序比较链（4.1 switch
        // lowering）且字段值不落槽 ⇒ 源形是 switch，不是 if-else 链（if 链在
        // 4.4 恒折 `cmpl $0,-0xNN(%ebp)` 并占具名槽）。
        switch (bonus.m_bestType)
        {
        case 1:
        case 2:
            if (bonus.m_selectedBonusExp < effectExp)
            {
                bonus.m_selectedBonusExp = effectExp;
                bonus.m_bestType = 4;
            }
            break;
        case 3:
            break;
        default:
            bonus.m_selectedBonusExp = effectExp;
            bonus.m_bestType = 4;
            break;
        }
    }

    bonus.m_channelBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_channelBonusRate);

    // 注意 ORIG 次序：燃烧疲劳块在择优之后执行并会把 m_bestType 覆写为 1。
    if (param.m_burningFatigueBonusExp != 0)
    {
        bonus.m_burningFatigueExp =   // ORIG 32 位乘除（mul+shr 魔术，无 __divdi3）
            (unsigned int)((unsigned int)param.m_burningFatigueBonusExp * exp / 100);
        bonus.m_bestType = 1;
    }

    bonus.m_serverChannelExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_serverChannelRate);
    // [SweepCParty2] 顺序照 ORIG 0x085ad6d6(0x28→+0x1c)/0x085ad708(0x24→+0x18)：
    // serverEventExp 先、premiumAdvExp 后（原写反，仅求值序，语义等价）。
    bonus.m_serverEventExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_serverEventExpRate);
    bonus.m_premiumAdvExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_premiumAdvantageRate);
    bonus.m_manageExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_manageBonus);
    bonus.m_growthPowerExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_growthPowerExpRate);
    bonus.m_premiumAddExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_premiumAddExpRate);

    unsigned int total =
        exp + bonus.m_rankWeightExp + bonus.m_avatarBonusExp +
        bonus.m_selectedBonusExp + bonus.m_channelBonusExp + bonus.m_premiumAdvExp +
        bonus.m_serverEventExp + bonus.m_burningFatigueExp + bonus.m_serverChannelExp +
        bonus.m_creatureBonusExp;

    bonus.m_upperMemberExp = user->gainExpAsUpperMember((int)exp);
    total += (unsigned int)bonus.m_upperMemberExp;
    total += (unsigned int)user->gainGuildSkillExp((int)exp);
    total += (unsigned int)user->gainPowerWarRewardExp((int)exp);
    total += bonus.m_periodItemExp + bonus.m_pcRoomSameUserExp + bonus.m_returnUserExp +
             bonus.m_eventCharacExp + bonus.m_manageExp + bonus.m_growthPowerExp +
             bonus.m_premiumAddExp;

    if (1000000 < total)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x28b1, "BLOOD_LOG : EXP(%u), ExpPerUser(%u)\n", total, expPerUser);
        LogManager::logFormat(
            1, "party.cpp",
// b28d-solo-CParty: DIFF 残 1 处 —— N18b/P254：局部 guildKey 与 callee 返回比较，4.4 恒折 cmp mem（已试 unsigned/enum 化无效）；106 vs 107
            __PRETTY_FUNCTION__,
            0x28c2,
            "BLOOD_LOG : BONUS EXP PARAMETER : %f, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n",
            (double)param.m_deadPenaltyRate,
            bonus.m_rankWeightExp, bonus.m_avatarBonusExp, bonus.m_creatureBonusExp,
            bonus.m_periodItemExp, bonus.m_pcRoomSameUserExp, bonus.m_selectedBonusExp,
            effectExp, bonus.m_channelBonusExp, bonus.m_burningFatigueExp,
            bonus.m_serverChannelExp, bonus.m_premiumAdvExp, bonus.m_serverEventExp,
            (unsigned int)bonus.m_upperMemberExp);
    }
    return total;
