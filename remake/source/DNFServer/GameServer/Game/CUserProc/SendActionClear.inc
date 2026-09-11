void APSystem::CUserProc::SendActionClear(CUser& user, unsigned short actionNo,
                                          _ActionGroupIndex group, bool flag,
                                          ENUM_PACKETCLASS packetClass)
{
    _MSG_ACTION_POINT_ACTION_CLEAR msg;
    // [b28c-4] ORIG 0x081223b4 赋值序 = m_actionNo → m_group → m_flag（直接字节存，
    // P136）；三元 `flag?1:0` 多出 cmp/je 常量岛（+4 条）。
    msg.m_actionNo = actionNo;
    msg.m_group = (int)group;
    msg.m_flag = flag;

    PacketGuard guard;
    if (packetClass == ENUM_PACKETCLASS_0)
    {
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x1f3);
    }
    else
    {
        ((InterfacePacketBuf*)&guard)->put_header(1, 0x228);
    }
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&msg, 0xb);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}