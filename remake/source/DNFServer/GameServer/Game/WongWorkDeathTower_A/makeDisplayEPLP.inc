void CDeathTower::CPacketHandler::makeDisplayEPLP(bool flag)
{
    put_header(0, 0x92);
    put_byte(flag ? 1 : 0);
    finalize(true);
