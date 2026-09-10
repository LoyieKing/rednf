bool CDispatchServerPacket::dispatchAuctionPacket(char* data)
{
    unsigned int packetId = (int)((nsl::PACKET_HEADER*)data)->getPacketID();
    int category = (int)((nsl::PACKET_HEADER*)data)->getCategory();
    if (category == 0x13)
    {
        dispatchPointPacket(data);
        return true;
    }

    switch (packetId)
    {
    case 0:  // 拍卖服务注册回执（包+0xe 结果码）
    {
        GARegistResultView* v = (GARegistResultView*)data;
        if (v->m_result == 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x128f), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_result = 0;
            sig->m_flag = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else if (v->m_result == 2)
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        else if (v->m_result == 1)
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a3, "AUCTION REGIST SERVER ID DUPLICATE");
        }
        else
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a7, "AUCTION REGIST UNHANDLED ERROR");
        }
        break;
    }
    case 1:  // 服务关闭通知
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12b2), true);
        (*guard)->operator<<(0xf9);
        (*guard)->operator<<(-1);
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_result = 0;
        sig->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（AG：两组价格来自包体）
    {
        GAPriceSrcView* v = (GAPriceSrcView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12cd), true);
        (*guard)->operator<<(0xf0);
        (*guard)->operator<<(v->m_src[1]);
        SIG_AUCTION_RESULT_ASK_PRICE* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
        sig->m_result = 0;
        sig->m_characNo = v->m_src[2];
        sig->m_itemIdx = v->m_src[3];
        for (int i = 0; i <= 2; ++i)
        {
            sig->m_priceCur[i] = v->m_src[i + 4];
            sig->m_priceTop[i] = v->m_src[i + 7];
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（AG：含 8×int 明细）
    {
        GARegistedNumView* v = (GARegistedNumView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12f0), true);
        (*guard)->operator<<(0xf1);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM>();
        sig->m_characNo = v->m_characNo;
        sig->m_count = v->m_count;
        sig->m_flagA = v->m_flagA;
        sig->m_flagB = v->m_flagB;
        sig->m_itemNums[0] = v->m_nums[0];
        sig->m_itemNums[1] = v->m_nums[1];
        sig->m_itemNums[2] = v->m_nums[2];
        sig->m_itemNums[3] = v->m_nums[3];
        sig->m_itemNums[4] = v->m_nums[4];
        sig->m_itemNums[5] = v->m_nums[5];
        sig->m_itemNums[6] = v->m_nums[6];
        sig->m_itemNums[7] = v->m_nums[7];
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 4:  // SIG_AUCTION_RESULT_REGIST
    {
        GARegistView* v = (GARegistView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1319), true);
        (*guard)->operator<<(0xf2);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_REGIST* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
        sig->m_characNo = v->m_characNo;
        sig->m_okFlag = v->m_okFlag;
        sig->m_errCode = v->m_errCode;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 5:  // SIG_AUCTION_RESULT_BIDDING（AG：无卖家名段）
    {
        GABiddingView* v = (GABiddingView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1336), true);
        (*guard)->operator<<(0xf3);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_BIDDING* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_BIDDING>();
        sig->m_characNo = v->m_characNo;
        sig->m_money = v->m_money;
        sig->m_flagA = v->m_flagA;
        sig->m_flagB = v->m_flagB;
        sig->m_field0e = v->m_field20;
        sig->m_bidPrice = v->m_bidPrice;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
    {
        GARegistView* v = (GARegistView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x135a), true);
        (*guard)->operator<<(0xf4);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
        sig->m_characNo = v->m_characNo;
        sig->m_okFlag = v->m_okFlag;
        sig->m_errCode = v->m_errCode;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 7:  // 拍卖列表（条目 0x89 字节）
    {
        GAItemListHeadView* v = (GAItemListHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1377), true);
        (*guard)->operator<<(0xf5);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        (*guard)->operator<<(v->m_u1b);
        int listBytes = (int)(v->m_cnt1f * 0x89 + 2);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1f);
            guard->put_binary(v->m_items, (int)(v->m_cnt1f * 0x89));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x1390, "onAUCTION_ITEM_LIST_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 8:  // 我的登记物品（条目 0x75 字节；AG：先写计数再拷贝）
    {
        GAMyRegistHeadView* v = (GAMyRegistHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13a3), true);
        (*guard)->operator<<(0xf6);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        int listBytes = (int)(v->m_cnt1b * 0x75 + 1);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1b);
            guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x75));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13bb, "onAUCTION_MY_REGISTED_ITEM_INFO_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 9:  // 我的竞购信息（条目 0x7d 字节）
    {
        GAMyBiddingHeadView* v = (GAMyBiddingHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13ce), true);
        (*guard)->operator<<(0xf7);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        int listBytes = (int)(v->m_cnt1b * 0x7d + 2);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1b);
            guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x7d));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13e6, "onAUCTION_MY_BIDDING_INFO_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xb:  // SIG_AUCTION_LOG_MESSAGE_AG（消费方 cUserHistoryLog::Auction）
    {
        GALogMsgView* v = (GALogMsgView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x142a), true);
        (*guard)->operator<<(0xf8);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_LOG_MESSAGE_AG* sig =
            guard->GetInBuffer<SIG_AUCTION_LOG_MESSAGE_AG>();
        sig->m_type = v->m_type;
        sig->m_keyWords[0] = v->m_keyLo;
        sig->m_keyWords[1] = v->m_keyHi;
        sig->m_arg09 = v->m_a1f;
        sig->m_arg0d = v->m_a23;
        sig->m_arg11 = v->m_a27;
        sig->m_arg15 = v->m_b2b;
        sig->m_arg16 = v->m_i2c;
        sig->m_arg1a = v->m_w30;
        sig->m_arg1c = v->m_i32;
        sig->m_arg20 = v->m_b36;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xc:  // 服务就绪通知（flag=1）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1447), true);
        (*guard)->operator<<(0xf9);
        (*guard)->operator<<(-1);
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_flag = 1;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xd:  // 就绪探测回执：成功→通知；否则续约定时器
    {
        GAReadyFlagView* v = (GAReadyFlagView*)data;
        if (v->m_ready != 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1464), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 1;
            sig->m_result = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        break;
    }
    case 0xe:  // SIG_AUCTION_RESULT_BUY_ITEM_APIECE（单件购买回执，sig 0x20d）
    {
        GABuyOneView* v = (GABuyOneView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1485), true);
        (*guard)->operator<<(0x20d);
        (*guard)->operator<<(v->m_s12);
        SIG_AUCTION_RESULT_BUY_ITEM_APIECE* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_BUY_ITEM_APIECE>();
        sig->m_result = v->m_s16;
        sig->m_characNo = v->m_s1a;
        sig->m_flagA = v->m_b1e;
        sig->m_flagB = v->m_b1f;
        sig->m_arg0a = v->m_s20;
        sig->m_price = v->m_s24;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    default:
        LogManager::logFormat(
            1, "DispatchServerPacket.cpp",
            __PRETTY_FUNCTION__,
            0x149c, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
        break;
    }
    return true;
