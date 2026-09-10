int channel_script_t::GetPvPChannelNumber(unsigned char channel, unsigned int channelNo) const
{
    // ORIG 0x088dcc28：外层 map(+0x18) 以 channel(serverNo) 查，未命中返回 -1；
    // 内层按 channelNo 升序遍历，每遇 PVP 频道（IS_PVP_CHANNEL(m_channelType)）
    // 计数 +1（-1 起步），命中 channelNo 即返回当前计数；遍历完未命中返回 -1。
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter =
        m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return -1;
    const std::map<int, channel_info_t>* pInner = &itOuter->second;
    int pvpIndex = -1;
    for (std::map<int, channel_info_t>::const_iterator it = pInner->begin();
         it != pInner->end(); ++it)
    {
        if (IS_PVP_CHANNEL(it->second.m_channelType))
            ++pvpIndex;
        // ORIG 0x88dccea：mov (%eax),%eax; cmp 0x10(%ebp),%eax（参数槽直折入 cmp，
        // 无寄存器中转 ⇒ 不加 (int) cast）
        if (it->first == channelNo)
            return pvpIndex;
    }
    return -1;
