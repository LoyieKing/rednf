bool CDispatchServerPacket::dispatchPointPacket(char* data)
{
    unsigned int packetId = (int)((nsl::PACKET_HEADER*)data)->getPacketID();
    int category = (int)((nsl::PACKET_HEADER*)data)->getCategory();
    if (category == 0x13)
    {
        switch (packetId)
        {
        case 0:  // 点券拍卖服务注册回执
        {
            GARegistResultView* v = (GARegistResultView*)data;
            if (v->m_result == 0)
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "DispatchServerPacket.cpp", 0x14b4), true);
                (*guard)->operator<<(0xf9);
                (*guard)->operator<<(-1);
                SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                    guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
                sig->m_result = 1;
                sig->m_flag = 1;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
                break;
            }
            if (v->m_result == 2)
            {
                Timer_CheckAuctionReady::RegistNextTimer();
                break;
            }
            if (v->m_result == 1)
            {
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x14c8, "AUCTION REGIST SERVER ID DUPLICATE");
                break;
            }
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x14cc, "AUCTION REGIST UNHANDLED ERROR");
            break;
        }
        case 1:  // 服务关闭通知
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x14d5), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 0;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（PG：价格组清零）
        {
            GAPriceSrcView* v = (GAPriceSrcView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x14e9), true);
            (*guard)->operator<<(0xf0);
            (*guard)->operator<<(v->m_src[1]);
            SIG_AUCTION_RESULT_ASK_PRICE* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
            sig->m_result = 1;
            sig->m_characNo = v->m_src[2];
            sig->m_itemIdx = v->m_src[3];
            for (int i = 0; i <= 2; ++i)
            {
                sig->m_priceCur[i] = 0;
                sig->m_priceTop[i] = 0;
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（PG：明细仅前 4 条）
        {
            GARegistedNumView* v = (GARegistedNumView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1502), true);
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
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 4:  // SIG_AUCTION_RESULT_REGIST
        {
            GARegistView* v = (GARegistView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1523), true);
            (*guard)->operator<<(0xf2);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_REGIST* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
            sig->m_characNo = v->m_characNo;
            sig->m_okFlag = v->m_okFlag;
            sig->m_errCode = v->m_errCode;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 5:  // SIG_AUCTION_RESULT_BIDDING（PG：附卖家名与尾字段）
        {
            GABiddingView* v = (GABiddingView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1536), true);
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
            sig->m_result = 1;
            strncpy(sig->m_sellerName, v->m_seller, strlen(v->m_seller));
            sig->m_field36 = v->m_field48;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
        {
            GARegistView* v = (GARegistView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x154e), true);
            (*guard)->operator<<(0xf4);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
            sig->m_characNo = v->m_characNo;
            sig->m_okFlag = v->m_okFlag;
            sig->m_errCode = v->m_errCode;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 7:  // 拍卖列表 PG（条目 0x89 字节）
        {
            GAItemListHeadView* v = (GAItemListHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1561), true);
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
                    0x1579, "onAUCTION_ITEM_LIST_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 8:  // 我的登记物品 PG（条目 0x75 字节；PG：双 put_binary 无计数写）
        {
            GAMyRegistHeadView* v = (GAMyRegistHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1584), true);
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
                guard->put_binary(&v->m_cnt1b, 1);
                guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x75));
            }        else
{
                (*guard)->operator<<((unsigned short)0);
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x159b, "onAUCTION_MY_REGISTED_ITEM_INFO_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 9:  // 我的竞购信息 PG（条目 0x7d 字节）
        {
            GAMyBiddingHeadView* v = (GAMyBiddingHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15a6), true);
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
                    0x15bd, "onAUCTION_MY_BIDDING_INFO_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xa:  // ORIG 静默忽略（决策树 jb→空块实测）
        {
            break;
        }
        case 0xb:  // SIG_AUCTION_LOG_MESSAGE_PG（消费方 cUserHistoryLog::AuctionPoint）
        {
            GALogMsgView* v = (GALogMsgView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15ce), true);
            (*guard)->operator<<(0xfa);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_LOG_MESSAGE_PG* sig =
                guard->GetInBuffer<SIG_AUCTION_LOG_MESSAGE_PG>();
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
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xc:  // 服务就绪通知（flag=1）
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15e5), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 1;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xd:  // 就绪探测回执：非零→通知；零→续约定时器
        {
            GAReadyFlagView* v = (GAReadyFlagView*)data;
            if (v->m_ready != 0)
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "DispatchServerPacket.cpp", 0x15fa), true);
                (*guard)->operator<<(0xf9);
                (*guard)->operator<<(-1);
                SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                    guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
                sig->m_result = 1;
                sig->m_flag = 1;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            }
            else
            {
                Timer_CheckAuctionReady::RegistNextTimer();
            }
            break;
        }
        case 0x2718:  // 转发类分支（ORIG 不可达但保留）：sig 0x2a5
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1613), true);
            (*guard)->operator<<(0x2a5);
            (*guard)->operator<<(v->m_s0a);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x2719:  // sig 0x2a6
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1620), true);
            (*guard)->operator<<(0x2a6);
            (*guard)->operator<<(v->m_s0f);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x271a:  // sig 0x2a7
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x162c), true);
            (*guard)->operator<<(0x2a7);
            (*guard)->operator<<(v->m_s0a);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x271b:  // sig 0x2a8
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1638), true);
            (*guard)->operator<<(0x2a8);
            (*guard)->operator<<(-1);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xe:  // SIG_AUCTION_OWNER_IS_VIP（sig 0x2da；ORIG 叶块排在转发分支后）
        {
            GAVipView* v = (GAVipView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1646), true);
            (*guard)->operator<<(0x2da);
            (*guard)->operator<<(v->m_s12);
            SIG_AUCTION_OWNER_IS_VIP* sig =
                guard->GetInBuffer<SIG_AUCTION_OWNER_IS_VIP>();
            sig->m_arg04 = v->m_s16;
            sig->m_arg08 = v->m_s1a;
            sig->m_arg0d = v->m_s1f;
            sig->m_arg11 = v->m_s23;
            strcpy(sig->m_ownerName, v->m_owner);
            sig->m_characNo = v->m_s12;
            sig->m_flagC = v->m_b1e;
            sig->m_arg26 = v->m_s38;
            sig->m_arg22 = v->m_s34;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        default:
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x166f, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
            break;
        }
    }
    return true;
