int Inter_MailBox_Req_Send::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b28c-30] N18a 封顶：ORIG 0x084d0161 段 `mov 0x8(%eax),%eax; mov %eax,%edx;
    // mov global,%eax; …`（字段装载 + edx 中转）；4.4 恒折单段
    // `mov 0x8(%eax),%edx`（unsigned 视图字段与 ENUM_SERVER_GROUP 形参视同型）。
    // 少 1 条 mov，P211 字段/callee 两旋钮均无中转形。
    Inter_MailBox_Req_SendPktView* pPkt = reinterpret_cast<Inter_MailBox_Req_SendPktView*>(data);
    if (pPkt->m_0 == 0)
    {
        // ORIG 0x084d017c：m_4 先物化 %ebx（跨 call 缓存），GetServerProxy 结果免槽直链
        register unsigned int mailNo = pPkt->m_4;   // ORIG +0x1a mov 0x4(%eax),%ebx
        // ORIG 中段三段 `mov 0x8(%eax),%eax; mov %eax,%edx; mov mgr,%eax`（4.1 先占
        // %eax 再挤 edx）；4.4 恒折单段 mem→edx（N18a 镜像），register asm 变体
        // 引 mgr 覆盖序错乱，blocked。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)pPkt->m_8)->SendNotifyNewMail(mailNo);
    }
    return 0;
