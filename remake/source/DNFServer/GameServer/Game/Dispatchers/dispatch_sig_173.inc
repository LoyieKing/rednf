int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08227256（报告 Dispatcher_Security_PI_HashModulation_C2C/dispatch_sig.md
    // 全文已读）：同 NonClient_GetResponse，switch 直比 %eax 无槽。
    switch (_NS_PI_2ND_RecvPacketIntegrityModulation_C2C(user, packet))
    {
        case 1:
        return LineFunc(0xe32f,
                        __PRETTY_FUNCTION__,
                        0, 0);
        case 2:
        return LineFunc(0xe331,
                        __PRETTY_FUNCTION__,
                        0, 0);
        default:
            break;
    }
    return 0;
