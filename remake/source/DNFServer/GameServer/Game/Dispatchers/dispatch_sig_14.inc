int Dispatcher_AskRematch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    char rematchFlag;
    PvP_Room* room;
    int matched;

    if (user->get_state() > 2)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
        return 0;
    rematchFlag = 0;
    if (!packet.get_byte(rematchFlag))
    {
        LineFunc(0xe3af,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    room = (PvP_Room*)user->GetPVPRoom();
    if (room == 0)
        return 0;
    if (room->m_matching == 0)
        return 0;
    matched = room->m_matching;
    // 真实虚调用：IMatch::OnAskRematch 在镜像声明序落 +0x68（MatchRoomList.h:78），
    // 与 ORIG 0x68 槽一致（§8.5 删函数指针桥→真实虚调）。matched 局部 int 存储口径不变。
    ((IMatch*)matched)->OnAskRematch(user, (int)rematchFlag);
    return 0;
