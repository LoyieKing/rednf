// ORIG T 0x85caaaa（报告 HandleError.md 逐指令）：栈上 CPacketBufPrivate 构造
// MakeErrorReturnPacket(code1,code2,code3) 后经 CUser::Send 下发，析构，返回 0。
int CPrivateStoreMgr::HandleError(CUser* user, int code1, int code2, int code3)
{
    CPacketBufPrivate packet;
    packet.MakeErrorReturnPacket(code1, code2, code3);
    user->Send(packet);
    return 0;
}