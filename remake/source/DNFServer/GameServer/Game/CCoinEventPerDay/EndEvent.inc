void CCoinEventPerDay::EndEvent()
{
    SetEventFlag(false);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x63, 0);
    tr("Coin Event Per Day End");
