// [b28d-solo-CDataManagerScripts] AE：①拆两个独立 null 守卫（P133，删 || 合并短路）；②删 vector 引用缓存（P183 逐点重读 info->m_pBonusRates）；③size_t i 头部前置 ⇒ info/-0xc、i/-0x10 与 ORIG 对齐；ORIG 0x088dce3c。
float channel_script_t::getBonusRate(unsigned char channel, unsigned int channelNo,
                                     unsigned int dungeonIndex) const
{
    // ORIG 0x088dce3c：getChannelInfo → 遍历 m_pBonusRates（vector<int>），命中则
    // 返回 (float)m_bonusRate / 100.0f；否则 0.0f。
    size_t i;
    channel_info_t* info = getChannelInfo(channel, channelNo);
    if (info == 0)
        return 0.0f;
    if (info->m_pBonusRates == 0)
        return 0.0f;
    for (i = 0; i < info->m_pBonusRates->size(); ++i)
    {
        if (info->m_pBonusRates->at(i) == dungeonIndex)
            // [bughunt13] ORIG 0x088dceb8 `flds 0x8e0c034` = 0x42c80000 = 100.0f（旧码 1000.0f 误）。
            return (float)info->m_bonusRate / 100.0f;
    }
    return 0.0f;
}