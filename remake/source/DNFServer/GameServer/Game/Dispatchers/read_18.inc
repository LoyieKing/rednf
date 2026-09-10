int Dispatcher_SkillInit::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e5934：读一个字节到 msg 的 m_0d
    MSG_SKILL_INIT& req = reinterpret_cast<MSG_SKILL_INIT&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x5f26, __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
