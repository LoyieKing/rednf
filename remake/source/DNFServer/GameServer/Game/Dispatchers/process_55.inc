// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d7ada orig/ours=154/164 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0xd(%eax),%eax | mov    0x15(%eax),%ecx | mov    0x1d(%eax),%edx
int Dispatcher_TitleBookGet::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TITLEBOOK_GET& m = reinterpret_cast<MSG_TITLEBOOK_GET&>(msg);

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // getTitle(user, category=+0x19, slot=+0x1d, itemIdx=+0x15, space=+0xd,
    //          outSlot=&+0x11)
    int ret = CTitleBook::getTitle(
        user,
        (ENUM_TITLE_BOOK_CATEGORY)m.m_category,
        m.m_field1d,
        m.m_field15,
        (ENUM_ITEMSPACE)m.m_itemSpace,
        m.m_field11);

    // ORIG 0x081d7b4d `cmpl $0,-0xc(%ebp); je 0x081d7c1a`（ret==0 跳后置块）
    // ⇒ fall-through 是 ret!=0 块：源码须把非零块写在 then。
    if (ret != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_category);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(1);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_field11);
        packet.put_int(m.m_category);
        packet.put_int(m.m_field1d);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}