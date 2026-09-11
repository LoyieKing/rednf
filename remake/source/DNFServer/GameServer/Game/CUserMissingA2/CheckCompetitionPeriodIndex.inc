// ----------------------------------------------------------------------------
// 10. ORIG T 0x081b2bc4 sz 0x59。G_CDataManager()->m_vec14：
// empty→0；!empty 且 size>=(signed)periodIdx→1；否则 0。movsbl 符号扩展。
// ----------------------------------------------------------------------------
bool CHARAC_LOAD_MERCENARY::CheckCompetitionPeriodIndex(char periodIdx) const
{
    // ORIG：G_CDataManager() 在 empty() 判与 size() 判两处各自重调（禁缓存，P183/P48）。
    // ORIG 全形复现：empty() 直判 test/jne 无 xor；idx 拓宽后置（size call 后
    // movsbl -0xc,%edx；cmp %edx,%eax；jae）；尾 !invalid choose（探针
    // /tmp/probe_l8j3.o g2 实证 28 条逐条全中）。
    if (G_CDataManager()->m_mercenarySystem.m_vec14.empty()
        || G_CDataManager()->m_mercenarySystem.m_vec14.size()
               < static_cast<unsigned int>(static_cast<int>(periodIdx)))
        return false;
    return true;
}