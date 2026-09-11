// [b28d-solo-Inter_DispatcherAradOnly] dispatch_sig 0x084e7f2c：
// ① 末参须写 `(int)((unsigned int)len - 4u)`——裸 `len - 4` 被 4.4 折成
//    `lea -0x4(%edx)`（N1 族）；unsigned 形复现 ORIG `mov 0x14(%ebp),%eax;
//    sub $0x4,%eax` 且把 `data + 4` 归到 %ecx（探针 /tmp/probe_sub.cpp g5；
//    语义等价：补码下 (unsigned)len-4u 与 len-4 同值）。
// ② 末尾虚调用目标寄存器 `call *%edx` 属 N7 敏感族（P230 补）：4.4.7 恒 %eax，
//    4.4.6 后端逐条复现 ORIG；本 TU 当前未路由，路由需求已上报 Main。
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
}