int Dispatcher_RequestCharacSkillInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e3914：char@0xd；越界（<0 或 >0x23）logFormat(1,...,0x5b20,...) 后 LineFunc(0x5b22)
    // [b28c-34] ORIG 0x081e396a `mov -0xc(%ebp),%eax; movzbl 0xd(%eax),%eax`：
    // 成员视图才折 disp（指针+常量下标 4.4 恒 add $0xd 两段）。
    struct ReqCharacSkillInfoMsgView
    {
        char m_pad[0xd];
        char m_idx;      // +0x0d
    };
    ReqCharacSkillInfoMsgView* v =
        reinterpret_cast<ReqCharacSkillInfoMsgView*>(&msg);
    if (!packet.get_byte(v->m_idx))
        return LineFunc(0x5b1b, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x081e396a/0x081e3978 每次重发 disp 读（无 characIdx 槽，P0 表达式直读）。
    if (v->m_idx < 0 || v->m_idx > 0x23)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x5b20,
                              "RequestCharacSkillInfo ReadPacket ERROR! : characIdx(%d)",
                              (int)v->m_idx);
        return LineFunc(0x5b22, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
