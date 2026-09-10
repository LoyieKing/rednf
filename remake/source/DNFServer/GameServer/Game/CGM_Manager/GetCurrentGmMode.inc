bool CGM_Manager::GetCurrentGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    // b28d-solo-CGM_Manager [P231 纯 register + bool 局部]：ORIG 0x082992f3
    // `movzbl (%eax),%ebx` 直落；探针 f_regbool 同法（register bool + return 即直落，
    // register char 恒多 `mov %eax,%ebx`）。纯 register 提示非绑定形（P0 合规）。
    register bool mode = m_gmMap[accId].m_mode;
    return mode;
