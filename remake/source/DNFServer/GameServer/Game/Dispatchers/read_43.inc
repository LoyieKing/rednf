// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081cd642
//   [N82→P0/P183] 根因=引用别名/msg 引用中间槽各占指针槽（帧 0x28→0x38）；
//   删引用别名改表达式直读 + 只留 pReq 单槽 ⇒ compare_tu clean 归零。
int Dispatcher_BossDieCheck::read(PacketBuf& packet, MSG_BASE& msg)
{
    // [b28d-solo-S206] P0：ORIG 单槽 -0xc（`mov 0x10(%ebp),%eax; mov %eax,-0xc`）
    //   ⇒ 原码只有 pReq 一个具名指针，无 msg 引用中间槽（引用形多 -0x10 存取对）。
    char* pReq = reinterpret_cast<char*>(&msg);
    // [b28d-solo-S206] P0/P183：ORIG 仅 -0xc 单槽（pReq）；三字段为表达式直读
    //   （`mov -0xc(%ebp),%eax; add $0xe/%0x10/%0x14,%eax`）。引用别名各占一个
    //   指针槽 ⇒ 帧 0x28→0x38、槽号漂移（compare_tu clean 实测归零）。
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(pReq + 0xe)))
    {
        return LineFunc(0x2578,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(pReq + 0x10)))
    {
        return LineFunc(0x2579,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>(pReq + 0x14)))
    {
        return LineFunc(0x257b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}