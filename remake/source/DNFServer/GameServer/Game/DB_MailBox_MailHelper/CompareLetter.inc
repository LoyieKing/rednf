// ============================================================================
// CompareLetter —— ORIG 0x0841d032：先比 +0x125（occ_time）严格小于；相等时
//   比首字段（无符号）严格小于。
// ============================================================================
bool CompareLetter(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
{
    if (((LetterOccView*)&a)->m_occ < ((LetterOccView*)&b)->m_occ)
        return 1;
    if (((LetterOccView*)&a)->m_occ == ((LetterOccView*)&b)->m_occ)
        return *(const unsigned int*)&a < *(const unsigned int*)&b;
    return 0;
}