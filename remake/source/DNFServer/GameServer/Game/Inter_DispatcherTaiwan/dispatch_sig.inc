int Inter_DispatcherTaiwan::dispatch_sig(CUser* user, char* data, int len)
{
    // b28d-lane12: AE（4.4.6 后端路由后 ndiff=0）。修法：末实参 len-4 →
    // len-sizeof(int)（unsigned 转换阻断 4.4 的 lea 折叠）复现 ORIG 0x084e8cbd
    // `mov 0x14(%ebp),%eax; sub $0x4,%eax`；4.4.6 后端复现 ORIG 0x084e8cba
    // `mov (%eax),%edx` + 0x084e8cdb `call *%edx`（4.4.7 为 N7：间接 call 目标
    // 恒 %eax，ndiff 24→18 NEAR；P230 补：换后端即翻）。探针 /tmp/l12_probe2.cpp
    // g5 逐条全等。
    int code = *(int*)data;   // ORIG mov (%eax),%eax 直读 4 字节（local_14）
    TaiwanInterfaceInterDispatcher* disp =
        m_disp.getDispatcher((TaiwanInternalPack::T)code);   // ORIG call getDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（getCurCharacNo 先求值存 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0x6dca, 5)
            ("[TaiwanDispatcher] Invalid internal packet code. Code:%d, Charac:%u)",
             code, user->getCurCharacNo());
        return 0x6dcb;   // ORIG mov $0x6dcb,%eax
    }
    return disp->dispatch_sig(user, data + 4, len - sizeof(int));   // ORIG 虚槽2（vptr+8）直调
}