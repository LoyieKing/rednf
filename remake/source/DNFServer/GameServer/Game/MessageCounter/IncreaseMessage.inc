void CMessageCounter::IncreaseMessage(unsigned int id, unsigned char type,
                                      unsigned short size, long long bytes)
{
    MessageKey key((int)id, type, size);
    std::map<MessageKey, MessageValue>::iterator it = m_map.find(key);
    double wOld = 0.0;
    double wNew = 0.0;
    if (it != m_map.end())
    {
        ++it->second.m_count;
        it->second.m_bytes += (unsigned long long)bytes;

        wOld = (double)(it->second.m_count - 1) / (double)it->second.m_count;
        wNew = 1.0 / (double)it->second.m_count;
        it->second.m_avg = it->second.m_avg * wOld + (double)bytes * wNew;
        return;
    }

    // 未命中：插入首记录（ORIG 0x0856fb3e 段）。左值实参 make_pair(key, zero)，
    // ORIG 发射 _ZSt9make_pairIRN15message_counter10MessageKeyERNS0_12MessageValueEE。
    MessageValue zero;
    zero.m_count = 1;
    zero.m_bytes = (unsigned long long)bytes;
    zero.m_avg = (double)bytes;
    m_map.insert(std::make_pair(key, zero));
