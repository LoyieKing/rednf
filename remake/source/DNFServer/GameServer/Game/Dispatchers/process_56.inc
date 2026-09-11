// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d774e orig/ours=154/154 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     081d788e <+0x140> | movl   $0x0,0x4(%esp) | lea    -0x1c(%ebp),%eax
int Dispatcher_TitleBookPut::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TITLEBOOK_PUT& m = reinterpret_cast<MSG_TITLEBOOK_PUT&>(msg);

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // putTitle(user, space=+0xd, slot=+0x11, itemIdx=+0x15, category=+0x19,
    //          titleSlot=+0x1d)
    int ret = CTitleBook::putTitle(
        user,
        (ENUM_ITEMSPACE)m.m_itemSpace,
        m.m_field11,
        m.m_field15,
        (ENUM_TITLE_BOOK_CATEGORY)m.m_field19,
        m.m_field1d);

    // ORIG 0x081d77bd `cmpl $0,-0xc(%ebp); je 0x081d788e`（ret==0 跳后置块）⇒ 非零块为 then。
    if (ret != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_field19);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(1);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_field11);
        packet.put_int(m.m_field19);
        packet.put_int(m.m_field1d);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}