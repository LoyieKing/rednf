// ORIG 0x0820adc6（报告 Dispatcher_AgreeEnchant/dispatch_sig.md）：
// get_packet(0) → MSG_STATIC_COMMAND*；curCharac==0 → LineFunc 0xa6f1；
// expertJob==0 / GetType()!=1 → 静默 return 0；否则 CEnchanter::OnAgreeEnchant。
// [b28c-31] AE：P0 内联表达式（删 job/enchanter 具名局部）+ cmd 槽 -0xc 回归。
int Dispatcher_AgreeEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_STATIC_COMMAND* cmd = (MSG_STATIC_COMMAND*)packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa6f1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [b28c-31] ORIG 0x0820ae2d：`call GetCurCharacExpertJob; test %eax,%eax; sete;
    // test; je`（无栈槽往返，P0）⇒ 内联表达式直判，勿命名 job 局部。
    if (user->GetCurCharacExpertJob() == 0)
    {
        return 0;
    }
    if (user->GetCurCharacExpertJob()->GetType() != 1)
    {
        return 0;
    }
    // 【batch-17 S池】ORIG 第 3 次 GetCurCharacExpertJob：转 CEnchanter 的源
    // 也重新取一次（0x081dxxxx `call GetCurCharacExpertJob; mov (%eax); ...`），
    // 复用 job 局部会少 1 call。
    // [b28c-31] ORIG 0x0820ae90：GetCurCharacExpertJob 结果直接作 receiver 压 (%esp)，
    // 无 enchanter 局部槽（P0/P183）。
    static_cast<expert_job::CEnchanter*>(user->GetCurCharacExpertJob())
        ->OnAgreeEnchant(user, cmd);
    return 0;
}