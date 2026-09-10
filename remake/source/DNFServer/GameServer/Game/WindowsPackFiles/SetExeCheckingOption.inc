void WindowsCodeProtectorServer::SetExeCheckingOption(long cfg, long e1,
                                                      long e2, long d1,
                                                      long d2)      // 0x085756ce
{
    // [N76 封顶] 除首字段外各 store 4.1 恒 RHS(param→edx) 先于 LHS(this 重载)，
    // 4.4 恒 LHS(this→eax) 先（probe 全试不可翻）；首字段两侧同序（eax 已持有）。
    m_cfgLong = cfg;
    m_exeMin = e1;
    m_exeMax = e2;
    m_dataMin = d1;
    m_dataMax = d2;
