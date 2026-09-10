void CCoinEventPerDay::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_coinPack = *(const unsigned int*)&param;
    cMyTrace(__FUNCTION__, 0x41, 0)(
        "Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ",
        m_coin01, m_coin02, m_coin03, m_coin04);
