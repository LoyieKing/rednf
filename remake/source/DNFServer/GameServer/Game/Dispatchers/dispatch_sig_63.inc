// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x0821b256 orig/ours=149/161 ndiff-blocks=24
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x20,%esp | jne    0821b26e <+0x18> | mov    $0x0,%ebx
int Dispatcher_DisguiseRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821b25e: `cmpl $0x0,0xc(%ebp); jne +0x18` 后即 `mov $0x0,%ebx; jmp 尾`
    //   ⇒ user==0 / state!=3 / ghost!=0 / assault!=0 各自独立早退（非 `||` 物化）；
    //   ORIG 0x0821b28d 直调 GetInstancePrivateStoreMgr 后**无判空**直接传 eax 进
    //   IsBusyPrivateStore（源码无 manager 具名局部，P48/P0）。
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0;
    }
    if (user->IsCurCharacGhost() != 0)
    {
        return 0;
    }
    if (user->GetAssaultPlace() != 0)
    {
        return 0;
    }
    // ORIG 0x0821b2e6 / 0x0821b32a：`lea -0x9(%ebp)`/`lea -0xc(%ebp)` 直取地址，
    //   无 `movb/movw $0x0` 预清 ⇒ 原码是未初始化的裸局部（P328⑤）。
    unsigned char kind;
    if (!packet.get_byte(kind))
    {
        return LineFunc(0xcf00,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short index;
    if (!packet.get_short(index))
    {
        return LineFunc(0xcf02,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->isDisguiseCharac() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd6, 0x12);
        return 0;
    }
    user->setDisguiseCharac(true, kind, index);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
}