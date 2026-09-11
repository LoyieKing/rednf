int Dispatcher_ChangeAnotherSkillTree::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        reinterpret_cast<MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x2ec2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}