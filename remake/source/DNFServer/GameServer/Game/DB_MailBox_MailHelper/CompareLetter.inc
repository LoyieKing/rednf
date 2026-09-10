bool CompareLetter(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
{
    if (((LetterOccView*)&a)->m_occ < ((LetterOccView*)&b)->m_occ)
        return 1;
    if (((LetterOccView*)&a)->m_occ == ((LetterOccView*)&b)->m_occ)
        return *(const unsigned int*)&a < *(const unsigned int*)&b;
    return 0;
