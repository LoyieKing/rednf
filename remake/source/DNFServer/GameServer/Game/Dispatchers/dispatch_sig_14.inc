// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08227862 orig/ours=62/74 ndiff-blocks=11
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: jle    08227887 <+0x25> | jne    0822788e <+0x2c> | mov    $0x1,%eax
int Dispatcher_AskRematch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char rematchFlag;
    PvP_Room* room;
    // ORIG 段直证：`cmp $0x2; jle true; call getCurCharacR; test; jne true;
    // mov $0x0,%eax; jmp done; true: mov $0x1,%eax; done: test %al,%al; je body`
    // ORIG 08227885 `jne +0x2c`（!=0 → mov $0x0 → gate=false）⇒ getCurCharacR()==0
    // 才是 gate=true 早退；P59/P222 极性：`state<=2 || getCurCharacR()==0`。
    // ORIG 在 %al 上直 test（无具名 gate 槽）⇒ P0/P183 去具名局部（+1→0）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return 0;
    rematchFlag = 0;
    if (!packet.get_byte(rematchFlag))
        return LineFunc(0xe3af, __PRETTY_FUNCTION__, 0, 0);
    room = (PvP_Room*)user->GetPVPRoom();
    // 正向包裹（ORIG 082278f6/08227903 两处各自 `je 尾部 return 0`，非早退复制）：
    // 真实虚调用 IMatch::OnAskRematch 在镜像声明序落 +0x68（MatchRoomList.h:78），
    // 与 ORIG 0x68 槽一致（§8.5 删函数指针桥→真实虚调）。ORIG 段直证 m_matching
    // 两处各自 `mov -0xc(%ebp),%eax; mov 0x6e4(%eax),%eax`（无 matched 槽）⇒ P48 重读。
    if (room != 0 && room->m_matching != 0)
        ((IMatch*)room->m_matching)->OnAskRematch(user, (int)rematchFlag);
    return 0;
}