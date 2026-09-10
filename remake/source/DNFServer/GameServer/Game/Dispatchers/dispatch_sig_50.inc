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
    char* msg = buy.m_pad;
    packet.get_byte(*(unsigned char*)(msg + 0x36));
    unsigned char count = *(unsigned char*)(msg + 0x36);
    if (count == 0 || count == 0 || 0x23 < count)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 1, 0,
                                              *(unsigned int*)(msg + 0x39), 1, 1, '\0');
        return 0;
    }
    PurchaseLimitItem::Info info;   // ORIG 0x081a405e 清零构造（-0x30 区）
    for (int i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (!packet.get_byte(*(unsigned char*)(msg + 0x37 + i * 0x1cf)))
        {
            return LineFunc(0x9172,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(*(char*)(msg + 0x38 + i * 0x1cf)))
        {
            return LineFunc(0x9175,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(*(unsigned int*)(msg + 0x39 + i * 0x1cf)))
        {
            return LineFunc(0x9178,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (info.reserved == 0)
        {
            ARAD::CheckAccountItem(user, *(unsigned int*)(msg + 0x39 + i * 0x1cf), info);
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
    Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x9187);
    CStreamGuard guard(stream, true);
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
