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
    // ORIG：getCurCharacNo/GetUID/get_acc_id 结果仅存局部（-0x1b1/-0x1b5/-0x1a1），
    // 从不写回 pkt（pkt 保持 C1 默认值）；仅调用副作用保留。
    int characNo = user->getCurCharacNo();
    int uid = user->GetUID();
    int type = 4;                                   // -0x50
    if (!packet.get_int(type) || type > 3)
    {
        return LineFunc(0x9f8e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int accId = user->get_acc_id();
    unsigned int nameLen = (unsigned int)strlen(user->getCurCharacName());  // -0x54
    unsigned int minLen = 0x1d;                     // -0x4c
    unsigned int copyLen = std::min(nameLen, minLen);
    strncpy(pkt.m_name, user->getCurCharacName(), copyLen);                 // +0x22
    unsigned char groupByte = 0;                    // -0x55
    packet.get_byte(groupByte);
    unsigned int group = (unsigned int)groupByte;   // -0x1a5（ORIG 不写 pkt）
    if (group > 0x64)
    {
        return LineFunc(0x9fa0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG：server_group 计算在 get_int(targetLen) 之前；
    //   group==0 → group=reporterGroup=env group；否则 reporterGroup=user group。
    unsigned int reporterGroup;                     // -0x1a9
    if (group == 0)
    {
        group = (unsigned int)G_CEnvironment()->get_server_group();
        reporterGroup = group;
    }
    else
    {
        reporterGroup = (unsigned int)user->GetServerGroup();
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
    if (reporterGroup == group &&
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