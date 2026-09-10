void StatisticsToObserveCrash::incUserLastCrashPacketID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it == m_crashPacketID.end())
    {
        // [b28d-62] P167：ORIG 直接构造 value_type（pair<int&,int> 模板 ctor），
        //   非 make_pair（多一对 make_pair+转换 ctor call）。
        m_crashPacketID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        // [b28d-62] P167：ORIG 走 operator*（deEv），非 operator->（ptEv）。
        (*it).second += 1;
    }
