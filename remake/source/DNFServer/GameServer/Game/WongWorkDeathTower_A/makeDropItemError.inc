// ORIG T 0x0845fea6：header(1,cmd)；byte(0)；byte(err)；byte(slot)；finalize(true)。
void CDeathTower::CPacketHandler::makeDropItemError(ENUM_CMDPACKET cmd, ENUM_ERROR err,
                                                    char slot)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    put_byte((int)(signed char)slot);
    finalize(true);
}