int Dispatcher_CompatibilityIndex::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e2664：local = check_error(user, msg, param)（不写 param）；
    // >0 返 0；<0 LineFunc(0x58f8,err,0)；==0 构造 Packet_Stat_Compatibility_Index，
    // 逐字段拷 msg 值后经 statistic 代理 SendPacket(pkt, 0x1e)。
    // [G1Dproc 2026-09-10] ORIG 第一句即 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   ⇒ msg 指针槽的声明在 check_error 之前（槽序 -0x10=msg、-0xc=err）。
    const char* m = reinterpret_cast<const char*>(&msg);
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
    // [G1Dproc 2026-09-10] ORIG 0x081e2664 逐偏移直证（pkt 基址 = -0x2e(%ebp)）：
    //   +0x0a acc_id（0x081e26d2 `mov %eax,-0x24(%ebp)`）、
    //   +0x10 *(int*)(msg+0x10)（-0x1e）、+0x0f msg+0xe（-0x1f）、+0x0e msg+0xd（-0x20）、
    //   +0x14 u16@msg+0x14（-0x1a）、+0x16 u16@msg+0x16（-0x18）、
    //   +0x1a u16@msg+0x1a（-0x14）、+0x18 u16@msg+0x18（-0x16）、
    //   +0x1c msg+0x1c（-0x12）、+0x1d msg+0x1d（-0x11）。
    //   原 +0x6/+0xa/+0xb/+0xc/+0x10/+0x12/+0x14/+0x16/+0x18/+0x19 全部少 4（整块错位），
    //   且 int@msg+0x10 与两个 byte 的语句顺序照 ORIG（+0x10 → +0x0f → +0x0e）。
    char* p = reinterpret_cast<char*>(&pkt);
    *reinterpret_cast<int*>(p + 0x0a) = user->get_acc_id();
    *reinterpret_cast<int*>(p + 0x10) = *reinterpret_cast<const int*>(m + 0x10);
    p[0x0f] = m[0xe];
    p[0x0e] = m[0xd];
    *reinterpret_cast<unsigned short*>(p + 0x14) =
        *reinterpret_cast<const unsigned short*>(m + 0x14);
    *reinterpret_cast<unsigned short*>(p + 0x16) =
        *reinterpret_cast<const unsigned short*>(m + 0x16);
    *reinterpret_cast<unsigned short*>(p + 0x1a) =
        *reinterpret_cast<const unsigned short*>(m + 0x1a);
    *reinterpret_cast<unsigned short*>(p + 0x18) =
        *reinterpret_cast<const unsigned short*>(m + 0x18);
    p[0x1c] = m[0x1c];
    p[0x1d] = m[0x1d];
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x1e);
    return 0;
