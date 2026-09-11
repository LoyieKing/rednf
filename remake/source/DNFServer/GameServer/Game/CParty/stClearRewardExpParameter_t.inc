// ============================================================================
// stClearRewardExpParameter_t / stClearRewardBonusExp_t 构造
//   ORIG W 0x0814a8b8 / 0x0814a800（逐 dword 清零；参数结构 +0x54/+0x55 字节
//   清零、+0x58/+0x5c 置 -1）。
// ============================================================================
stClearRewardExpParameter_t::stClearRewardExpParameter_t()
{
    m_deadPenaltyRate = 0.0f;
    m_rankBonusWeight = 0.0f;
    m_avatarBonus = 0.0f;
    m_creatureBonus = 0.0f;
    m_pcRoomBurningRate = 0.0f;
    m_expDoubleRate = 0.0f;
    m_channelBonusRate = 0.0f;
    m_burningFatigueBonusExp = 0;
    m_serverChannelRate = 0.0f;
    m_premiumAdvantageRate = 0.0f;
    m_serverEventExpRate = 0.0f;
    m_periodExpItemRate = 0.0f;
    m_globalEffectRate = 0.0f;
    m_pcRoomSameUserBonus = 0;
    m_manageBonus = 0.0f;
    m_returnUserPartyBonus = 0.0f;
    m_eventCharacExpRatio = 0.0f;
    m_growthPowerExpRate = 0.0f;
    m_premiumAddExpRate = 0.0f;
    m_reserved4c = 0;
    m_reserved50 = 0;
    m_reserved54 = 0;
    m_reserved55 = 0;
    m_deadMemberCount = -1;
    m_rankBonusIdx = -1;
}