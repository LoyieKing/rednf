bool CMissionList_Charac::isUserNewbie() const
{
    static std::bitset<256> zeroBit(0);   // ORIG bitset<256>::bitset(unsigned long)（C1Em），非默认构造
    if (empty_progress() && (zeroBit == m_clearedFlags))
        return true;
    else
        return false;
