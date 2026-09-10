void WongWork::CDeathTower::CPacketHandler::makePickupItemHeader(unsigned short a,
                                                                unsigned short b)
{
    put_header(0, 0x27);
    put_short((int)b);
    put_short((int)a);
