float channel_script_t::getBonusRate(unsigned char channel, unsigned int channelNo,
                                     unsigned int dungeonIndex) const
{
    // ORIG 0x088dce3c：getChannelInfo → 遍历 m_pBonusRates（vector<int>），命中则
    // 返回 (float)m_bonusRate / 1000.0f；否则 0.0f。
    size_t i;
    channel_info_t* info = getChannelInfo(channel, channelNo);
    if (info == 0)
        return 0.0f;
    if (info->m_pBonusRates == 0)
        return 0.0f;
    for (i = 0; i < info->m_pBonusRates->size(); ++i)
    {
        if (info->m_pBonusRates->at(i) == dungeonIndex)
            return (float)info->m_bonusRate / 1000.0f;
    }
    return 0.0f;
