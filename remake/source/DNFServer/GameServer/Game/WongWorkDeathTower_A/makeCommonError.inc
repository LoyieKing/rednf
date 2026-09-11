// ORIG T 0x0845fe4c：header(1,cmd)；byte(0)；byte(err)；finalize(true)。
void CDeathTower::CPacketHandler::makeCommonError(ENUM_CMDPACKET cmd, ENUM_ERROR err)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    finalize(true);
}