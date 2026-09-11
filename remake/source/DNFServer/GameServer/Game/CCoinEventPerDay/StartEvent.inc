// ---------------------------------------------------------------------------
// ORIG 0x0810ab96：SetEventFlag(true) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x38,0) ("Coin Event Per Day Start!")。
void CCoinEventPerDay::StartEvent()
{
    SetEventFlag(true);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x38, 0);
    tr("Coin Event Per Day Start!");
}