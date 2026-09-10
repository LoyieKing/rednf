int Dispatcher_MailBox_Send::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMailboxSendParam& out = static_cast<stMailboxSendParam&>(param);

    out.m_4 = check_error(user, msg);                           // +0x04 错误码
    if (out.m_4 > 0)
        return 0;
    if (out.m_4 < 0)
        return LineFunc(0x23ce,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());

    if (mbGuildKey(msg) != 0) {                                 // ---- 公会邮件分支 ----
        const int fee = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa624);  // +0xa624 公会邮件费（推断）
        CInventory* invW = user->getCurCharacInvenW();
        invW->use_money(fee, static_cast<eMoneySubReason>(0x16), true);

        CGuildServerProxy* proxy =
            reinterpret_cast<CServerProxyMgr<CGuildServerProxy>*>(GlobalData::s_guild_proxy_mgr)
                ->GetServerProxy(user->GetServerGroup());
        proxy->SendGuildMail(static_cast<unsigned int>(user->getCurCharacNo()),
                             static_cast<unsigned int>(user->get_charac_guildkey()),
                             mbTitleLen(msg),
                             mbText(msg));
        return 0;
    }

    // ---- 附带金币时先走客户端确认回执（certify）路径 ----
    bool needCertify = false;
    if (mbGold(msg) != 0) {
        // ORIG 0x081cca92: needCertify = user->reqSendMailCertify();
        needCertify = user->reqSendMailCertify() != 0;
    }
    if (needCertify) {
        // ORIG 0x081ccaa6: user->set_MailMsgData(&msg) 后返回 0（等待回执）
        user->set_MailMsgData(reinterpret_cast<MSG_MAILBOX_SEND*>(&msg));
        return 0;
    }

    // ---- DB 收件人校验请求 ----
    int rc = 0;
    {
        MSG_CHECK_MAIL_RECEIVER chk;
        chk.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0, MSG_MAILBOX_SEND>(
            reinterpret_cast<const MSG_MAILBOX_SEND&>(msg));
        rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, mbName(msg), &chk);
    }
    if (rc != 0) {
        out.m_4 = rc;                                           // 校验失败码透传
    }
    else {
        // 交易金币计数：普通低稀有度附件才计入特殊标记
        unsigned char specialMark = 0;
        if (mbItemId(msg) != 0) {
            CItem* item = G_CDataManager()->find_item(static_cast<int>(mbItemId(msg)));
            if (item != 0) {
                // ORIG 经 vtable+0x10 间接调用（= IsAvatarItem 槽，ORIG vtable 已核实）
                if (!item->IsAvatarItem() && !(item->get_rarity() > 1))
                    specialMark = 1;
            }
        }
        if (mbItemId(msg) == 0)
            specialMark = 1;
        // ORIG 0x081ccc26: user->IncrementTradeGold(mbGold(msg), specialMark);
        user->IncrementTradeGold(mbGold(msg), static_cast<int>(specialMark));
    }

    // ---- 邮件金币上限统计（SIG_FIND_FACTORY_HUB 入 NETWORK 统计队列）----
    {
        const unsigned short usedFatigue = user->getCurCharacUsedFatigue();
        const int rnd = get_rand_int(0x19);
        const int money = user->getCurCharacInvenR()->get_money();
        // ORIG 浮点序：(long long)(money * (0.7 + (float)((double)rnd * 0.01)))
        // （.rodata 0x08bd6338=0.01、0x08bd6340=0.7，已从二进制核实）
        const double factor = 0.7 + static_cast<double>(static_cast<float>(static_cast<double>(rnd) * 0.01));
        const unsigned long long goldCap =
            static_cast<unsigned long long>(static_cast<double>(money) * factor);
        if (goldCap <= static_cast<unsigned long long>(mbGold(msg))) {
            Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x241d);
            CStreamGuard guard(stream, true);
            guard << static_cast<int>(0x18a);                   // 统计包命令字
            guard << static_cast<int>(user->GetUID());
            SIG_FIND_FACTORY_HUB* sig = guard.GetInBuffer<SIG_FIND_FACTORY_HUB>();
            memset(sig, 0, 0x24);                               // SIG 总长 0x24（当前头为 pad 视图）
            *reinterpret_cast<unsigned int*>(sig) = user->get_acc_id();          // +0x00 accId
            strncpy(reinterpret_cast<char*>(sig) + 4, mbName(msg),
                    static_cast<size_t>(mbLen(msg)));                            // +0x04 收件人名
            *(reinterpret_cast<char*>(sig) + 0x21) = 0;                          // +0x21 疲劳标记清零
            const int fatigueRoll = get_rand_int(0x14);
            if (fatigueRoll + 0x78 <= static_cast<int>(static_cast<unsigned int>(usedFatigue)))
                *(reinterpret_cast<char*>(sig) + 0x21) = 1;              // +0x21 疲劳标记
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
    return 0;
