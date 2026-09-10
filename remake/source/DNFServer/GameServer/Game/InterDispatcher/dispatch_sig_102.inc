int Inter_ReportMannerless::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N：残余为 ORIG 用 %esi 承载跨分支标志/返回（ORIG[141] mov $0x1,%esi 等）
    //   与我方 %eax + 具名 local_10 的寄存器分配差；未定位到对应具名局部，未改。
    Inter_ReportMannerlessPktView* pPkt = reinterpret_cast<Inter_ReportMannerlessPktView*>(data);
    if (pPkt == 0)
        return 0;

    CUser* local_10 = 0;
    if (user == 0 ||
        pPkt->m_e != user->get_charac_no(-1))
    {
        local_10 = G_CGameManager()->GetUserByAccId(
            pPkt->m_1e);
    }
    else
    {
        local_10 = user;
    }

    if (pPkt->m_15e == 'o')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x62);
        return 0;
    }
    else if (pPkt->m_15e == 'c')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x5f);
        return 0;
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x326d), true);
        guard.operator*()->operator<<(0xca);
        guard.operator*()->operator<<(-1);
        guard.operator->()->put_binary(data, 0x15f);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

        if (local_10 != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x8c);
            packet.put_byte(1);
            packet.finalize(true);
            local_10->Send(packet);
        }
    }
    return 0;
