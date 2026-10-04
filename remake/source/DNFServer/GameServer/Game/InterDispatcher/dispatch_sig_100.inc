// [b28d-lane2] verdict=DIFF ndiff=384（ORIG 0x084d1c8a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+27。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84d1c8a）：本轮三处 findString 内联（P48，p167 M 归零）；余 N91 EH landing（EXTRA _Unwind_Resume×2）+P30/N4。
int Inter_ReqChangeCharacName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N（本轮未改）：213 行残差以「ORIG 用 data 的栈副本 <S>
    //   （`mov <S>,%eax; add $0x4,%eax`）vs 我方 `mov 0x10(%ebp),%eax; lea 0x4(%eax),%edx`」
    //   的表达式形态、以及 `xor/mov $0x31ae,%eax` 返回常量路径为主；无单点可判定依据，留 N。
    SIG_CHANGE_CHARAC_NAME* chg = reinterpret_cast<SIG_CHANGE_CHARAC_NAME*>(data);
    // ORIG 0x084d1c8a 报告 docs/class_func_reports/Inter_ReqChangeCharacName/dispatch_sig.md
    // 两段全读。msg：+0 characNo / +4 newName / +0x24 errFlag / +0x28 rewardFlag /
    // +0x2c guildKey。
    if (user->get_state() <= 1)
        return 0;

    // §P0：ORIG 每处使用都重读 msg+0x0（084d1ce8/1d21/1de2/1e68 等，无槽往返）⇒ 不设具名 characNo。
    if (chg->m_arg24 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a,
                                 static_cast<unsigned char>(
                                     chg->m_arg24 & 0xff));
        user->SetCharacViewStateByCharacNo(chg->m_characNo, 0xff);
        user->UpdateCharacView();
        return 0;
    }

    if (!user->setCharacName(chg->m_characNo, data + 4))
        return 0x31ae;

    {   // 成功应答包
        PacketGuard packet;
        packet.put_header(1, 0x8a);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    user->SetCharacViewStateByCharacNo(chg->m_characNo, 1);
    {   // 雇佣兵名同步（ORIG std::string(data+4) 局部 + UpdateCharacNameInMercenary）
        std::string name(data + 4);
        user->UpdateCharacNameInMercenary(
            static_cast<unsigned int>(chg->m_characNo), name);
    }
    user->UpdateCharacView();

    // ORIG 0x0840b96b 两段：第一段 if(guildKey) guild 1 次 else monitor 1 次；
    // 第二段 if(guildKey) { guild 1 次 + monitor 1 次 }。NEW 原实现把第一段
    // guild 重复两次且缺第二段，属转写错误。
    if (chg->m_arg2c != 0)
    {
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                  static_cast<unsigned int>(chg->m_characNo),
                                  static_cast<unsigned int>(chg->m_arg2c),
                                  data + 4);
    }
    else
    {
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitor->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                    static_cast<unsigned int>(chg->m_characNo),
                                    static_cast<unsigned int>(chg->m_arg2c),
                                    data + 4);
    }
    if (chg->m_arg2c != 0)
    {
        CGuildServerProxy* proxyG =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxyG->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                   static_cast<unsigned int>(chg->m_characNo),
                                   static_cast<unsigned int>(chg->m_arg2c),
                                   data + 4);
        CMonitorServerProxy* monitorM =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitorM->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                     static_cast<unsigned int>(chg->m_characNo),
                                     static_cast<unsigned int>(chg->m_arg2c),
                                     data + 4);
    }

    if (chg->m_arg28 == 0)
    {   // 改名活动奖励期（ORIG 2007-05 活动阈值 0x464a1f50）
        const long curSec = GlobalData::s_systemTime_.getCurSec();
        if (curSec > 0x464a1f50)
            return 0;
        const short level =
            (short)user->getCharacLevelByCharacNo(chg->m_characNo);
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
                static_cast<unsigned int>(chg->m_characNo),
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
                static_cast<unsigned int>(chg->m_characNo),
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
                static_cast<unsigned int>(chg->m_characNo),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
    }

    GlobalData::s_event_manager->changeCharacName(
        user, chg->m_characNo, data + 4,
        chg->m_arg28);
    return 0;
}