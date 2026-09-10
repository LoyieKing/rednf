int Dispatcher_Equipslot_Switch::read(PacketBuf& packet, MSG_BASE& msg)
{
    // [b28d-solo-S206] P0：ORIG 单槽 -0xc，无 msg 引用中间槽（同 BossDieCheck::read）。
    char* pReq = reinterpret_cast<char*>(&msg);
    // [b28d-solo-S206] P0/P183：ORIG 仅 -0xc 单槽（pReq），三字段表达式直读
    //   （`mov -0xc(%ebp),%eax; add $0xd/%0xe/%0xf,%eax`）；引用别名各占槽
    //   ⇒ 帧 0x28→0x38（compare_tu clean 实测归零）。
    if (!packet.get_byte(pReq[0xd]))
    {
        return LineFunc(0x3e12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(pReq[0xe]))
    {
        return LineFunc(0x3e14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(pReq[0xf]))
    {
        return LineFunc(0x3e16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
