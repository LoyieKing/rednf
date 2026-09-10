void make_noti_packetheader_jpn(PacketGuard& guard, ENUM_NOTIPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header(0, (int)code);
    guard.put_byte((int)normalized);
