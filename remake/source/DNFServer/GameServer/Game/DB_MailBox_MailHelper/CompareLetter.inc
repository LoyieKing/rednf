// ============================================================================
// CompareLetter —— ORIG 0x0841d032：先比 +0x125（occ_time）严格小于；相等时
//   比首字段（无符号）严格小于。
// ============================================================================
bool CompareLetter(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
{
    if (a.m_period < b.m_period)
        return 1;
    if (a.m_period == b.m_period)
        return a.m_letterId < b.m_letterId;
    return 0;
}