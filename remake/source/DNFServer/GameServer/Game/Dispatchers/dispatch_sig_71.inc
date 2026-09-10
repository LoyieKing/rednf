int Dispatcher_Security_IPScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // 探针实证：`if (call()==1)` 4.4 产 sete 物化；switch 单 case 保 ORIG 直比 cmp/jne。
    switch (_NS_IPScan(user, packet))
    {
        case 1:
            return LineFunc(0xde94,
                            __PRETTY_FUNCTION__,
                            0, 0);
        default:
            return 0;
    }
