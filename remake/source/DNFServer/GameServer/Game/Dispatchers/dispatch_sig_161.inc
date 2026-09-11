int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0826c3xx：`call; cmp $0x5,%eax; ja default; mov <A>(,%eax,4),%eax; jmp *%eax`
    // —— switch 直接作用于 call 结果（技巧37 免槽），case 升序 + default 置末
    // （技巧34：default 岛落物理尾、前一 case 的 jmp 指向它）。
    switch (_NS_PacketIntegrity(user, packet))
    {
    case 1:
        return LineFunc(0xded7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 2:
        return LineFunc(0xded9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 3:
        return LineFunc(0xdedb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 4:
        return LineFunc(0xdedd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 5:
        return LineFunc(0xdedf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    default:
        return 0;
    }
}