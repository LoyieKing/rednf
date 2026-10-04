// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084d0130）：混合残差；gap0。
int Inter_MailBox_Req_Send::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b28c-30] N18a 封顶：ORIG 0x084d0161 段 `mov 0x8(%eax),%eax; mov %eax,%edx;
    // mov global,%eax; …`（字段装载 + edx 中转）；4.4 恒折单段
    // `mov 0x8(%eax),%edx`（unsigned 视图字段与 ENUM_SERVER_GROUP 形参视同型）。
    // 少 1 条 mov，P211 字段/callee 两旋钮均无中转形。
    SIG_MAILBOX_SEND_NEW_MAIL* mail = reinterpret_cast<SIG_MAILBOX_SEND_NEW_MAIL*>(data);
    if (mail->m_reply.m_result == 0)
    {
        // ORIG 0x084d017c：m_4 先物化 %ebx（跨 call 缓存），GetServerProxy 结果免槽直链
        register unsigned int mailNo = mail->m_reply.m_mailIdx;   // ORIG +0x1a mov 0x4(%eax),%ebx
        // ORIG 中段三段 `mov 0x8(%eax),%eax; mov %eax,%edx; mov mgr,%eax`（4.1 先占
        // %eax 再挤 edx）；4.4 恒折单段 mem→edx（N18a 镜像），register asm 变体
        // 引 mgr 覆盖序错乱，blocked。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)mail->m_reply.m_group)->SendNotifyNewMail(mailNo);
    }
    return 0;
}