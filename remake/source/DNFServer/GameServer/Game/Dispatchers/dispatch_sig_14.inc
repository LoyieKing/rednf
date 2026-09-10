int Dispatcher_AskRematch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    char rematchFlag;
    PvP_Room* room;
    // ORIG 段直证：`cmp $0x2; jle true; call getCurCharacR; test; jne true;
    // mov $0x0,%eax; jmp done; true: mov $0x1,%eax; done: test %al,%al; je body`
    // ⇒ 单一 bool 表达式（`state<=2 || getCurCharacR()!=0`），4.1 物化岛；
    // 原 if/else 双赋值形退化为 setg/setne 两条 setcc（+3 insn）。
    gate = user->get_state() <= 2 || user->getCurCharacR() != 0;
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
    // 真实虚调用：IMatch::OnAskRematch 在镜像声明序落 +0x68（MatchRoomList.h:78），
    // 与 ORIG 0x68 槽一致（§8.5 删函数指针桥→真实虚调）。ORIG 段直证 m_matching
    // 两处各自 `mov -0xc(%ebp),%eax; mov 0x6e4(%eax),%eax`（无 matched 槽）⇒ P48 重读。
    ((IMatch*)room->m_matching)->OnAskRematch(user, (int)rematchFlag);
    return 0;
