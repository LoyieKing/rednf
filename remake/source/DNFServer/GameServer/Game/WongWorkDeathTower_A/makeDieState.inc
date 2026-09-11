// ORIG T 0x08460176：header(0,0x20)；short(uid)；byte(0)；finalize(true)。
void CDeathTower::CPacketHandler::makeDieState(unsigned short uid)
{
    put_header(0, 0x20);
    put_short((int)uid);
    put_byte(0);
    finalize(true);
}