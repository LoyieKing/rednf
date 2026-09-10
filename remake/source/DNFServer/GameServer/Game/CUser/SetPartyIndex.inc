void CUser::SetPartyIndex(short index)
{
    lock();
    m_field8d004 = index;
    unlock();
