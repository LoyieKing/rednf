channel_info_t* channel_script_t::getChannelInfo(unsigned char channel, unsigned int idx) const
{
    // ORIG 0x88dcd44/0x88dcd4f：入口按"内层(-0x1c)先、外层(-0x20)后"顺序默认
    // 构造两个迭代器；find 结果落临时 -0x30 再单字赋值（0x88dcd80-0x88dcd83、
    // 0x88dcde8-0x88dcdeb）⇒ 声明+赋值两段式，inner 声明在前。
    std::map<int, channel_info_t>::const_iterator itInner;
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter;
    itOuter = m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return 0;
    itInner = itOuter->second.find((int)idx);
    if (itInner == itOuter->second.end())
        return 0;
    return const_cast<channel_info_t*>(&itInner->second);
