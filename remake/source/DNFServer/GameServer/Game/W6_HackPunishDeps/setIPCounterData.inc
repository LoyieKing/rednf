void CAutoPunishRuleHackTypeMgr::setIPCounterData(unsigned int a, unsigned int b,
                                                  char* ip, unsigned int cnt)
{
    // b28d-solo-setIPCounterData: ORIG make_pair 实例为
    // make_pair<unsigned int&, HackIPCounter_t>（第一参取形参 a/b 地址，
    // 第二参匿名右值临时）——旧写法 (unsigned short)a + 具名 entry 两个旋钮全错。
    // ORIG 0x080f9218。
    m_ipCounterMap.insert(std::make_pair(a, HackIPCounter_t(a, b, ip, cnt)));

    if (b != 0)
    {
        m_ipCounterMap.insert(std::make_pair(b, HackIPCounter_t(a, b, ip, cnt)));
    }
