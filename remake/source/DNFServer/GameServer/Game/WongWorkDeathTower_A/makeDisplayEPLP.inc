// ORIG T 0x084604b0：header(0,0x92)；byte(flag)；finalize(true)。
void CDeathTower::CPacketHandler::makeDisplayEPLP(bool flag)
{
    put_header(0, 0x92);
    put_byte(flag ? 1 : 0);
    finalize(true);
}