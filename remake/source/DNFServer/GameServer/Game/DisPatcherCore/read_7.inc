int DisPatcher_DeleteCharac::read(PacketBuf& packet, MSG_BASE& msg)

{
    // 视图：+0x0e int 名长（ORIG 0x81c... `mov -0xc(%ebp),%eax; mov 0xe(%eax),%eax` 单级字段
    // 装载）+0x12 名字区（`add $0x12`）。单一指针局部 -0xc（原 nameLen 引用局部多余槽）。
    #pragma pack(push, 1)
    struct View : MSG_BASE
    {
        signed char m_slot;
        int m_nameLen;
        char m_name[0x1e];
        char m_pad30[8];
    };
    #pragma pack(pop)
    static_assert(sizeof(View) == 0x38, "DeleteCharac read view");
    View* req = reinterpret_cast<View*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req->m_slot)))
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
