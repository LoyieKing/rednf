void CCoinEventPerDay::StartEvent()
{
    SetEventFlag(true);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x38, 0);
    tr("Coin Event Per Day Start!");
