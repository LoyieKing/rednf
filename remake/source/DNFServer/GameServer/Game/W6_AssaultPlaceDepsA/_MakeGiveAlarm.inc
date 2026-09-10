void CAssaultPlace::_MakeGiveAlarm(PacketGuard* p)
{
    p->put_header(0, 0x8b);
    p->finalize(true);
