// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0 模式字节。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 60 = our 60（compare_tu AE 口径）。旧 B24D
//   「N」与 b28c-13「N18a」判词作废：同 TurnGmMode，register bool 局部即直落 %ebx。
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
}