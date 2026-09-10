void CDeathTower::CPacketHandler::makeDropItemError(ENUM_CMDPACKET cmd, ENUM_ERROR err,
                                                    char slot)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    put_byte((int)(signed char)slot);
    finalize(true);
