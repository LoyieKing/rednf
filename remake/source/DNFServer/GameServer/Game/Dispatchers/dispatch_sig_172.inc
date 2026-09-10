int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08224e64（报告 Dispatcher_Security_NonClient_GetResponse/dispatch_sig.md
    // 全文已读）：switch(调用结果) 直比 %eax（无 result 槽，sub $0x18 帧）。
    switch (_NS_NonClient_GetResponse(user, packet))
    {
        case 1:
        return LineFunc(0xdf88,
                        __PRETTY_FUNCTION__,
                        0, 0);
        case 2:
        return LineFunc(0xdf8a,
                        __PRETTY_FUNCTION__,
                        0, 0);
        default:
            break;
    }
    return 0;
