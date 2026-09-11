int Dispatcher_PutItemForEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820b09a
    MSG_STATIC_COMMAND* msg = (MSG_STATIC_COMMAND*)packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa744,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // b28c-33：ORIG 该处无具名局部（寄存器直比 sete 形），值缓存局部使 4.4 退化为
    // mov+cmpl 槽比 ⇒ 改内联表达式（P0/P225）。
    if (user->GetCurCharacExpertJob() == 0)
    {
        return 0;
    }
    if (user->GetCurCharacExpertJob()->GetType() != 1)
    {
        return LineFunc(0xa74b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // b28c-33：ORIG 无 enchanter 具名局部（GetCurCharacExpertJob 返回值直入 (%esp)）
    // ⇒ 内联链式调用，msg 局部回到 -0xc。
    static_cast<expert_job::CEnchanter*>(user->GetCurCharacExpertJob())
        ->OnPutItemForEnchant(user, msg);
    return 0;
}