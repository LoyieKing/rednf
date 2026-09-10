int Inter_ReqChangeCharacName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReqChangeCharacNamePktView* pPkt = reinterpret_cast<Inter_ReqChangeCharacNamePktView*>(data);
    // ORIG 0x084d1c8a 报告 docs/class_func_reports/Inter_ReqChangeCharacName/dispatch_sig.md
    // 两段全读。msg：+0 characNo / +4 newName / +0x24 errFlag / +0x28 rewardFlag /
    // +0x2c guildKey。
    if (user->get_state() <= 1)
        return 0;

    // §P0：ORIG 每处使用都重读 msg+0x0（084d1ce8/1d21/1de2/1e68 等，无槽往返）⇒ 不设具名 characNo。
    if (pPkt->m_24 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a,
                                 static_cast<unsigned char>(
                                     pPkt->m_24 & 0xff));
        user->SetCharacViewStateByCharacNo(pPkt->m_0, 0xff);
        user->UpdateCharacView();
        return 0;
    }

    if (!user->setCharacName(pPkt->m_0, data + 4))
        return 0x31ae;

    {   // 成功应答包
        PacketGuard packet;
        packet.put_header(1, 0x8a);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    user->SetCharacViewStateByCharacNo(pPkt->m_0, 1);
    {   // 雇佣兵名同步（ORIG std::string(data+4) 局部 + UpdateCharacNameInMercenary）
        std::string name(data + 4);
        user->UpdateCharacNameInMercenary(
            static_cast<unsigned int>(pPkt->m_0), name);
    }
    user->UpdateCharacView();

    // ORIG 0x0840b96b 两段：第一段 if(guildKey) guild 1 次 else monitor 1 次；
    // 第二段 if(guildKey) { guild 1 次 + monitor 1 次 }。NEW 原实现把第一段
    // guild 重复两次且缺第二段，属转写错误。
    if (pPkt->m_2c != 0)
    {
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                  static_cast<unsigned int>(pPkt->m_0),
                                  static_cast<unsigned int>(pPkt->m_2c),
                                  data + 4);
    }
    else
    {
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitor->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                    static_cast<unsigned int>(pPkt->m_0),
                                    static_cast<unsigned int>(pPkt->m_2c),
                                    data + 4);
    }
    if (pPkt->m_2c != 0)
    {
        CGuildServerProxy* proxyG =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxyG->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                   static_cast<unsigned int>(pPkt->m_0),
                                   static_cast<unsigned int>(pPkt->m_2c),
                                   data + 4);
        CMonitorServerProxy* monitorM =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitorM->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                     static_cast<unsigned int>(pPkt->m_0),
                                     static_cast<unsigned int>(pPkt->m_2c),
                                     data + 4);
    }

    if (pPkt->m_28 == 0)
    {   // 改名活动奖励期（ORIG 2007-05 活动阈值 0x464a1f50）
        const long curSec = GlobalData::s_systemTime_.getCurSec();
        if (curSec > 0x464a1f50)
            return 0;
        const short level =
            (short)user->getCharacLevelByCharacNo(pPkt->m_0);
        if (level <= 0)
            return 0x31b7;

        // ORIG 三处 ReqDBSendNewSystemMail 各自独立 findString（p167 M=findString×2 ⇒ 无缓存）。
        if (level > 0x31)
        {
            Inven_Item item;                       // 改名卡 36 号 ×5
            item.m_addInfo = 0x24;
            item.m_field1 = 2;
            item.m_addInfo2 = 5;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
        if (level > 0x27)
        {
            Inven_Item item;                       // 16 号 ×1
            item.m_addInfo = 0x10;
            item.m_field1 = 2;
            item.m_addInfo2 = 1;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
        {
            Inven_Item item;                       // 1 号 ×10（无条件）
            item.m_addInfo = 1;
            item.m_field1 = 2;
            item.m_addInfo2 = 0xa;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
    }

    GlobalData::s_event_manager->changeCharacName(
        user, pPkt->m_0, data + 4,
        pPkt->m_28);
    return 0;
