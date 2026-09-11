// ---------------------------------------------------------------------------
// ORIG 0x0810aca8：SetEventFlag(false) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x63,0) ("Coin Event Per Day End")。
void CCoinEventPerDay::EndEvent()
{
    SetEventFlag(false);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x63, 0);
    tr("Coin Event Per Day End");
}