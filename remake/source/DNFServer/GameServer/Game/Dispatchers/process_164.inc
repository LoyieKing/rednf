int Dispatcher_CompatibilityIndex::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e2664：local = check_error(user, msg, param)（不写 param）；
    // >0 返 0；<0 LineFunc(0x58f8,err,0)；==0 构造 Packet_Stat_Compatibility_Index，
    // 逐字段拷 msg 值后经 statistic 代理 SendPacket(pkt, 0x1e)。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x58f8,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    Packet_Stat_Compatibility_Index pkt;   // ORIG W ctor 0x0822c61e（局部帧 -0x2e 起）
    // 帧偏移换算（base -0x2e）：+0x6 acc_id、+0xa msg[0xd]、+0xb msg[0xe]、
    // +0xc *(int*)(msg+0x10)、+0x10 u16@msg+0x14、+0x12 u16@msg+0x16、
    // +0x14 u16@msg+0x1a、+0x16 u16@msg+0x18、+0x18 msg[0x1c]、+0x19 msg[0x1d]
    char* p = reinterpret_cast<char*>(&pkt);
    const char* m = reinterpret_cast<const char*>(&msg);
    *reinterpret_cast<int*>(p + 6) = user->get_acc_id();
    p[0xa] = m[0xd];
    p[0xb] = m[0xe];
    *reinterpret_cast<int*>(p + 0xc) = *reinterpret_cast<const int*>(m + 0x10);
    *reinterpret_cast<unsigned short*>(p + 0x10) =
        *reinterpret_cast<const unsigned short*>(m + 0x14);
    *reinterpret_cast<unsigned short*>(p + 0x12) =
        *reinterpret_cast<const unsigned short*>(m + 0x16);
    *reinterpret_cast<unsigned short*>(p + 0x14) =
        *reinterpret_cast<const unsigned short*>(m + 0x1a);
    *reinterpret_cast<unsigned short*>(p + 0x16) =
        *reinterpret_cast<const unsigned short*>(m + 0x18);
    p[0x18] = m[0x1c];
    p[0x19] = m[0x1d];
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x1e);
    return 0;
