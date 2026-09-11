// ----------------------------------------------------------------------------
// setFullIPCounterData —— ORIG T 0x080f946a (0x109)：对 +0x40 全 IP 表，
// 结构同 setIPCounterData。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setFullIPCounterData(unsigned int a, unsigned int b,
                                                      char* ip, unsigned int cnt)
{
    // b28d-solo-setFullIPCounterData: 同 setIPCounterData——ORIG make_pair 实例为
    // make_pair<unsigned int&, HackFullIPCounter_t>（形参 a/b 取址 + 匿名右值临时）。
    // ORIG 0x080f946a。
    m_fullIPCounterMap.insert(std::make_pair(a, HackFullIPCounter_t(a, b, ip, cnt)));

    if (b != 0)
    {
        m_fullIPCounterMap.insert(std::make_pair(b, HackFullIPCounter_t(a, b, ip, cnt)));
    }
}