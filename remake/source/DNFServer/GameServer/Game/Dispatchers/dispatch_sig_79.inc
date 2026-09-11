// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08207b52 orig/ours=126/131 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,0xc(%esp) | movl   $0x0,0x8(%esp) | movl   $&_ZZN27Dispatcher_ReplyGuildInvite12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(
int Dispatcher_ReplyGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char reply = 0;
    // ORIG 0x0821..：[20] 段 LineFunc(0xa278) 就地早退（`call; mov %eax,%ebx; jmp 尾`），
    //   我方旧形 `if (gate) {...} return LineFunc(...)` 把 LineFunc 沉到函数尾 ⇒ 指令序错位。
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() == 0))
    {
        return LineFunc(
            0xa278,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    {
        if (!packet.get_byte(reply))
        {
            return LineFunc(
                0xa27c,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
        {
            PacketGuard response;
            response.put_header(1, 0x9b);
            response.put_byte(0);
            response.put_int(99);
            response.finalize(true);
            user->Send(response);
            return 0;
        }
        const int secuRet =
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xc);
        if (secuRet != 0)
        {
            reply = static_cast<unsigned char>(
                GlobalData::s_pSecuProtectionField->GetOppositeErr(secuRet));
        }
        const unsigned int characNo = user->getCurCharacNo();
        // ORIG [114]：GetServerProxy 结果经 %eax 直传 SendReplyGuildInvite
        //   （无 proxy 具名指针槽）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendReplyGuildInvite(characNo, reply);
        return 0;
    }
}