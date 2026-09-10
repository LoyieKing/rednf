void CDeathTower::CPacketHandler::makeCommonError(ENUM_CMDPACKET cmd, ENUM_ERROR err)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    finalize(true);
