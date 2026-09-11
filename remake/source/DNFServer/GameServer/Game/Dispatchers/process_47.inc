// ORIG 0x081e2f78（报告 Dispatcher_InformNotice/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x598c)；==0 handleInformNoticeFlag。
int Dispatcher_InformNotice::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e2f78（报告 Dispatcher_InformNotice/process.md 全文已读）：
    // param@-0x10 先存；check_error 结果直存 (param+4)；msg 绑定@-0xc 只出现在
    // handle 分支内（store+回读两行都位于该块）。
    InformNoticeData& out = static_cast<InformNoticeData&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x598c,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    MSG_INFORM_NOTICE& req = reinterpret_cast<MSG_INFORM_NOTICE&>(msg);   // ORIG 块内 -0xc 存储
    user->handleInformNoticeFlag((unsigned int)req.m_noticeFlag);
    return 0;
}