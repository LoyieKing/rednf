int Inter_EmergencyPatchMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_EmergencyPatchMessagePktView* pPkt = reinterpret_cast<Inter_EmergencyPatchMessagePktView*>(data);
    // ORIG +0x1b：`test %al,%al`+`je` 无 setne 物化 ⇒ 直接真值测试（`!= 0` 会插入物化两行）。
    if (GameserverDemonManagerInstance()->IsProhibitConnectState())
        return 0;
    GameserverDemonManagerInstance()->SetUserProhibitFlag(true);
    // ORIG +0x3b：串基址取包基址槽 -0xc(%ebp)（经具名 pPkt，不回落参数槽 0x10）。
    GameserverDemonManagerInstance()->SetNoticeMessage(
        pPkt->m_a, reinterpret_cast<char*>(pPkt) + 0xb);
    GameserverDemonManagerInstance()->Notice10MinShutDownMessage();
    return 0;
}