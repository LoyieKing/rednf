int Inter_DispatcherAradOnly::dispatch_sig(CUser* user, char* data, int len)
{
    int code = *(int*)data;   // ORIG mov (%eax),%eax 直读 4 字节（local_14）
    ARAD::DISPATCHER::Arad_IInterDispatcher* disp =
        m_disp.GetDispatcher((ARAD::ENUM_INTERNALPACKET_JPN)code);   // ORIG call GetDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（getCurCharacNo 先求值存 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0x6ba2, 5)
            ("[DISPATCHER_ARAD] Invalid Internal Packet Code. (code:%d, charac:%u)",
             code, user->getCurCharacNo());
        return 0x6ba3;   // ORIG mov $0x6ba3,%eax
    }
    return disp->dispatch_sig(user, data + 4, (int)((unsigned int)len - 4u));   // ORIG 虚槽2（vptr+8）直调
