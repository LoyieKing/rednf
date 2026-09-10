bool CMailBox::InsertLetterKeepCount(unsigned int letterId)
{
    return m_letterKeepCount
        .insert(std::make_pair(letterId, true)).second;
