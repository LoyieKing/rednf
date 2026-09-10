unsigned char CGM_Manager::TurnGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    if (m_gmMap[accId].m_mode)
        m_gmMap[accId].m_mode = 0;
    else
        m_gmMap[accId].m_mode = 1;
    // b28d-solo-CGM_Manager [P231 纯 register + bool 局部]：ORIG 0x0829923e
    // `movzbl (%eax),%ebx`（零扩展直落 callee-saved 返回值缓存）。4.4.7 探针
    // （/tmp/b28d_s78_probe1.cpp f_regbool）实证：`register bool` 局部 + `return mode;`
    // 即直落 %ebx；`register unsigned char` 恒多一条 `mov %eax,%ebx`。纯 register
    // 存储类提示非绑定形（P0 合规，禁 `register asm`）。
    register bool mode = m_gmMap[accId].m_mode;
    return mode;
