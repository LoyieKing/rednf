int Dispatcher_LimitNpcBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x55ae,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    LimitNpcBuyItemInfo info;   // local_3a（栈对象 0x26）
    info.m_itemIdReq = user->GetUID();             // local_30（+0xa）
    info.m_dbid = user->get_acc_id();              // local_2c（+0xe）
    info.m_charNo = (unsigned int)user->get_charac_no(-1);   // local_28（+0x12）
    info.m_itemId = *(unsigned int*)((char*)&msg + 0xd);     // local_24（+0x16）
    info.m_count = 1;                              // local_20（+0x1a）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&info), 0x26);
    return 0;
