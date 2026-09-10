void CDeathTower::CPacketHandler::makeFinishLoading()
{
    put_header(0, 0x1e);
    put_int(0);
    finalize(true);
