int DisPatcher_UdpCharacteristic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned short local_e = 0;
    unsigned char local_f = 0;
    if (packet.get_short(local_e) != true)
    {
        return LineFunc(0xce12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(local_f) != true)
    {
        return LineFunc(0xce14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [Lane38 2026-09-10] 判据①/⑧ 块布局：ORIG 0x0821ac7c 分派为 **switch 比较链**
    // `movzwl -0xa(%ebp),%eax; movzwl %ax,%eax; cmp $1,%eax; je +0x193;
    //  cmp $2,%eax; je +0x2d3; test %eax,%eax; jne +0x3aa`（fall-through = case 0，
    // default 在 case2 之后）；if-else 链的 4.4 O0 形是就近 `jne` + 各块 fall-through
    // （我方旧形态 jcc 目标差 −106/+134/+133）。双 `movzwl` 提升亦为 switch 表达式
    // int 提升直证（if 比较为 `cmp $1,%ax` 16 位）。语义等价改写。
    switch (local_e)
    {
    case 0:
    {
        Packet_Party_Result_Statistic local_1d;
        unsigned short tmp_succTry; unsigned short tmp_totalTry;   // packed 字段不可绑引用，经临时读取
        if (packet.get_short(tmp_succTry) != true)
        {
            return LineFunc(0xce1d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(tmp_totalTry) != true)
        {
            return LineFunc(0xce1e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_1d.m_successPartyTry = tmp_succTry;
        local_1d.m_totalPartyTry = static_cast<unsigned short>(tmp_totalTry + tmp_succTry);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_1d), 0xe);
        break;
    }
    case 1:
    {
        unsigned short local_26 = 0;
        unsigned short local_24 = 0;
        if (packet.get_short(local_26) != true)
        {
            return LineFunc(0xce30,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(local_24) != true)
        {
            return LineFunc(0xce31,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (local_f == '\0')
        {
            partyPingStat(user, local_26, local_24);
        }
        else if (local_f == '\x01')
        {
            pvpPingStat(user, local_26, local_24);
        }
        else
        {
            if (local_f != '\x02')
            {
                return LineFunc(0xce44,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            fairPvpPingStat(user, local_26, local_24);
        }
        break;
    }
    case 2:
    {
        Packet_Abnormal_Exit_Statistic local_30;
        unsigned short tmp_succClear; unsigned short tmp_totalClear;   // packed 字段不可绑引用，经临时读取
        if (packet.get_short(tmp_succClear) != true)
        {
            return LineFunc(0xce68,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(tmp_totalClear) != true)
        {
            return LineFunc(0xce69,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_30.m_successDungeonClear = tmp_succClear;
        local_30.m_totalDungeonClear = static_cast<unsigned short>(tmp_totalClear + tmp_succClear);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_30), 0xe);
        break;
    }
    default:
        // ORIG 0x0821af8a（+0x3aa，case2 块之后）：非 0/1/2 值 → LineFunc(0xce76)。
        return LineFunc(0xce76,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
