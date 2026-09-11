// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081fe578 orig/ours=333/340 ndiff-blocks=63
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN25Dispatcher_BuyAutomatItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x28(%ebp),%eax | movl   $"pUser d
int Dispatcher_BuyAutomatItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081fe578（PacketDispatcher_Impl_1.cpp 0x9145 区）
    if (user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x9145, 0);
        trace("pUser doesn't exist in Dispatcher_BuyAutomatItem::dispatch_sig");
        return LineFunc(0x9146,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // get_state()<=2 或 getCurCharacR()==0 → 拒绝
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x914a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->CheckInTrade())
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, '\0');
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x44);
    if (secu != 0)
    {
        int err = (secu == 0x7b) ? 0x76 : 0x86;
        GlobalData::s_pCeraShop->ProcessError(user, (unsigned short)err, 0, 0, 1, 1, '\0');
        return 0;
    }
    // 局部 MSG_BUY_CERASHOP_ITEM（0x3f84），+0x36 计数、条目步长 0x1cf：
    //   +0x37 槽位 uchar / +0x38 标签 char / +0x39 productNo uint
    MSG_BUY_CERASHOP_ITEM buy;
    // ORIG 段直证：各访问直接 `movzbl -0x3f86(%ebp),%eax` / `add $0x36` 前的裸基址
    // （常量偏移折叠），无 `char* msg` 具名槽（原具名令帧 +4 并整段槽位下移 4）。
    packet.get_byte(*(unsigned char*)(buy.m_pad + 0x36));
    unsigned char count = *(unsigned char*)(buy.m_pad + 0x36);
    if (count == 0 || count == 0 || 0x23 < count)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 1, 0,
                                              *(unsigned int*)(buy.m_pad + 0x39), 1, 1, '\0');
        return 0;
    }
    PurchaseLimitItem::Info info;   // ORIG 0x081a405e 清零构造（-0x30 区）
    for (int i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (!packet.get_byte(*(unsigned char*)(buy.m_pad + 0x37 + i * 0x1cf)))
        {
            return LineFunc(0x9172,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(*(char*)(buy.m_pad + 0x38 + i * 0x1cf)))
        {
            return LineFunc(0x9175,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(*(unsigned int*)(buy.m_pad + 0x39 + i * 0x1cf)))
        {
            return LineFunc(0x9178,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (info.reserved == 0)
        {
            ARAD::CheckAccountItem(user, *(unsigned int*)(buy.m_pad + 0x39 + i * 0x1cf), info);
        }
    }
    if (info.reserved == 0)
    {
        if (!GlobalData::s_pItemVendingMachine->BuyItem(user, buy))
        {
            return LineFunc(0x9183,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        return 0;
    }
    // 异步分支：经 DB_Q 流转发给后台购买处理
    // [DispA2 P0/P183] ORIG 0x081fe98a `call StreamPool::Acquire` 后紧接
    //   `movl $0x1,0x8(%esp); mov %eax,0x4(%esp)`（回值直入 ctor），无 stream 具名槽。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "PacketDispatcher_Impl_1.cpp", 0x9187),
                       true);
    **guard << 0x1fa;                                  // << int
    **guard << (int)user->GetUID();                    // asm 调 << int
    **guard << user->get_acc_id();                     // << unsigned int
    **guard << user->get_charac_no(-1);                // << int
    **guard << (unsigned int)info.reserved;            // << unsigned int
    **guard << (short)info.purchaseType;               // << short
    **guard << (short)info.limitCount;                 // << short
    // ORIG：GetInBuffer 经 operator->（0x080c8c4e）后调用（P103）。
    MSG_BUY_CERASHOP_ITEM* out = guard.operator->()->GetInBuffer<MSG_BUY_CERASHOP_ITEM>();
    memcpy(out, &buy, 0x3f84);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}