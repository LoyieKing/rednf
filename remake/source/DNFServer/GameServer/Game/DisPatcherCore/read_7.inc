int DisPatcher_DeleteCharac::read(PacketBuf& packet, MSG_BASE& msg)

{
    // 视图：+0x0e int 名长（ORIG 0x81c... `mov -0xc(%ebp),%eax; mov 0xe(%eax),%eax` 单级字段
    // 装载）+0x12 名字区（`add $0x12`）。单一指针局部 -0xc（原 nameLen 引用局部多余槽）。
    MSG_DELETE_CHARACTER* req = reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    if (!packet.get_byte(req->m_slot))
    {
        return LineFunc(0xa26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req->m_nameLen))
    {
        return LineFunc(0xa29,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(req->m_name, 0x1e, req->m_nameLen))   // +0x12 名字区
    {
        return LineFunc(0xa30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
