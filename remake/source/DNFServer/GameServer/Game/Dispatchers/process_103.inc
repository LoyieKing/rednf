// ORIG 0x081cce50（报告 Dispatcher_MailBox_Extract/process.md）：
// msg+0xd（unsigned）为邮箱取件索引。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cce50 orig/ours=48/40 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,%edx | mov    -0x10(%ebp),%eax
int Dispatcher_MailBox_Extract::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMailboxExtractParam& p = static_cast<stMailboxExtractParam&>(param);
    p.m_4 = check_error(user, msg);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        LineFunc(0x245d,
                 __PRETTY_FUNCTION__,
                 p.m_4, user->get_acc_id());
    }
    else
    {
        MSG_MAILBOX_EXTRACT_ITEM& mref = reinterpret_cast<MSG_MAILBOX_EXTRACT_ITEM&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
        WongWork::CMailBoxHelper::GetPackege(user, mref.m_mailIdx);
    }
    return 0;
}