// ==== Dispatcher_FatigueAttendance::read (ORIG 0x081dafa4) ====
int Dispatcher_FatigueAttendance::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_FATIGUE_ATTENDANCE& req = reinterpret_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // ORIG：get_int(&msg+0xd)；失败走 LineFunc(0x49b3)。
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x49b3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}