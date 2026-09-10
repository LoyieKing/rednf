void StatisticsNatType::IncreaseNatType(int natA, int natB, unsigned char count)
{
    NAT_TYPE_PEER_TO_PEER key(natA, natB);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.find(key);

    RESULT_COUNT* rc = 0;
    if (it != m_counts.end())
    {
        rc = it->second;
    }
    else
    {
        rc = new RESULT_COUNT();
        // ORIG 0x0860c924 段：pair 转发 ctor `_ZNSt4pairIK…EEC1IRS1_RS4_EEOT_OT0_`
        // （K&/V& 直构）⇒ 源码为 value_type(key, rc) 直构；make_pair 会多出
        // `_ZSt9make_pairIR…E` + move 转换 ctor 两次调用（P192 配套）。
        m_counts.insert(std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::value_type(key, rc));
    }

    if (count != 0)
    {
        ++rc->m_countA;
    }
    else
    {
        ++rc->m_countB;
    }
