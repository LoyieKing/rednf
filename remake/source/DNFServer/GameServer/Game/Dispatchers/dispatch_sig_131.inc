// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112/N197②] ORIG 0x08206124 orig/ours=232/231
//   prologue 差：ORIG `push %ebx`（值跨 call 保活占 callee-saved）vs 我方直接
//   sub $0x1f8,%esp；4.4 未升格（P197② 边界 / N82 寄存器分配跨版本）。语义已对齐。
//   首差: push   %ebx | sub    $0x1f8,%esp
int Dispatcher_ReportMannerlessUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9f84,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    Packet_Mannerless_User_Accusation pkt;
    // [DNF-FIX 2026-10-08 c30] ORIG 0x0820619f..0x08206313（pkt@-0x1bf）：结果全部写入 pkt 本体——
    //   +0x0e(-0x1b1) characNo、+0x0a(-0x1b5) uid、+0x12(-0x1ad) type、+0x1e(-0x1a1) acc_id、
    //   +0x1a(-0x1a5) 被举报方服务器组（客户端字节/0 时取本服）、+0x16(-0x1a9) 举报方服务器组；
    //   旧注释「仅存局部、从不写回 pkt」误读（这些槽即 pkt 内偏移），监控包缺字段发出。
    //   本 TU 成员名沿用旧偏移命名：m_characNo=+0x0a、m_type=+0x0e、m_serverGroup=+0x12、
    //   m_reporterGroup=+0x16、m_accIdLo=+0x1a、m_accIdHi=+0x1e。
    pkt.m_type = user->getCurCharacNo();            // +0x0e
    pkt.m_characNo = (int)user->GetUID();           // +0x0a
    int type = 4;                                   // -0x50
    if (!packet.get_int(type) || type > 3)
    {
        return LineFunc(0x9f8e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    pkt.m_serverGroup = type;                       // +0x12
    pkt.m_accIdHi = (unsigned int)user->get_acc_id();   // +0x1e
    unsigned int nameLen = (unsigned int)strlen(user->getCurCharacName());  // -0x54
    unsigned int minLen = 0x1d;                     // -0x4c
    unsigned int copyLen = std::min(nameLen, minLen);
    strncpy(pkt.m_name, user->getCurCharacName(), copyLen);                 // +0x22
    unsigned char groupByte = 0;                    // -0x55
    packet.get_byte(groupByte);
    pkt.m_accIdLo = (unsigned int)groupByte;        // +0x1a
    if ((int)pkt.m_accIdLo > 0x64)
    {
        return LineFunc(0x9fa0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (pkt.m_accIdLo == 0)
    {
        pkt.m_accIdLo = (unsigned int)G_CEnvironment()->get_server_group();
        pkt.m_reporterGroup = (int)pkt.m_accIdLo;   // +0x16
    }
    else
    {
        pkt.m_reporterGroup = (int)user->GetServerGroup();
    }
    unsigned int targetLen = 0;                     // -0x5c
    if (!packet.get_int(targetLen))
    {
        return 0;
    }
    if (!packet.get_str(pkt.m_targetName, 0x1e, (int)targetLen))            // +0x40
    {
        return 0;
    }
    if (Packet_Mannerless_User_Accusation::isNeedTypeForChattingMsg(type))
    {
        unsigned int chatLen = 0;                   // -0x60
        if (!packet.get_int(chatLen))
        {
            return 0;
        }
        if (!packet.get_str(pkt.m_chatMsg, 0x100, (int)chatLen))            // +0x5e
        {
            return 0;
        }
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    Inven_Item item = inven->GetInvenSlot(0, 0xb);
    if (item.m_addInfo == 0x677e)
    {
        pkt.m_result = 0x70;
    }
    if ((unsigned int)pkt.m_reporterGroup == pkt.m_accIdLo &&
        targetLen == nameLen &&
        strncmp(pkt.m_name, pkt.m_targetName, targetLen) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 3);
        return 0;
    }
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket((char*)&pkt, 0x15f);
    return 0;
}