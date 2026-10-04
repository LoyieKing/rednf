void CUser::SetPartyIndex(short index)
{
    lock();
    m_partyIndex = index;
    unlock();
}