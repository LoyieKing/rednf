// ORIG MakeCmdpacketRenameCreature @ 0x083405fa：clear → put_header(1,0x67)
// → put_byte(1) → put_short(param1) → put_byte(param2) → finalize(true)。
void CPacketRespondent::MakeCmdpacketRenameCreature(int param1, int param2)
{
    clear();
    put_header(1, 0x67);
    put_byte(1);
    put_short(param1);
    put_byte(param2);
    finalize(true);
}