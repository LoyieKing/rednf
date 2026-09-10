void internal_stream(CStreamGuard& guard, TaiwanInternalPack::T packet, int uid)
{
    guard.operator*()->operator<<(0x34e);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(static_cast<int>(packet));
