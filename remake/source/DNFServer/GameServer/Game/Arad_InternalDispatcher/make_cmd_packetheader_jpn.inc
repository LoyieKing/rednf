void make_cmd_packetheader_jpn(PacketGuard& guard, ENUM_CMDPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header(1, (int)code);
    guard.put_byte((int)normalized);
