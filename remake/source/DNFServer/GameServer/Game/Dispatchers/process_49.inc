// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cc196 orig/ours=81/79 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
int Dispatcher_MailBox_Open::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cc196
    stMailboxOpenParam& out = static_cast<stMailboxOpenParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x22c8,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }

    WongWork::CMailBox* mailBox = (WongWork::CMailBox*)user->GetMailBox();
    if (mailBox->GetNotLoadedMailCount() != 0)
    {
        // ORIG 0x081cc23c `call GetRecvSize` → 0x081cc241 `mov %eax,%ebx`（无槽）与
        // 0x081cc249 `call GetLoadedLetterCount` → 0x081cc24e `lea (%ebx,%eax,1),%eax`
        // ⇒ 删 recvSize/loadedCount 两具名 int（各一对 store/load，帧 +8）⇒ P0/P183。
        if (mailBox->GetRecvSize() + mailBox->GetLoadedLetterCount() <= 0x13)
        {
            WongWork::CMailBoxHelper::ReqDBLoadMail(user,
                                                    (unsigned int)user->getCurCharacNo());
            out.m_4 = 0x16;
            return 0;
        }
    }
    // ORIG：未发 DB 请求时再次调 GetNotLoadedMailCount 写 param+8
    out.m_8 = mailBox->GetNotLoadedMailCount();

    return 0;
}