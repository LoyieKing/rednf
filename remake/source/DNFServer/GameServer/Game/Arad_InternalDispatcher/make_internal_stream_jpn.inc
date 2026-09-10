void make_internal_stream_jpn(CStreamGuard& guard, ENUM_INTERNALPACKET_JPN code, int param)
{
    guard.operator*()->operator<<((int)0x2dc);
    guard.operator*()->operator<<((int)param);
    guard.operator*()->operator<<((int)code);
