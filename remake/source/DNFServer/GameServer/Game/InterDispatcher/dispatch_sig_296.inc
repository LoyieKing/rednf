int Inter_CollectItemsRewardBroadcast::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_CollectItemsRewardBroadcastPktView* pPkt = reinterpret_cast<Inter_CollectItemsRewardBroadcastPktView*>(data);

    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(0xb);
    packet.put_byte((unsigned int)pPkt->m_12);
    // ORIG +0x60..+0x67：`movzbl 0x12(%eax),%eax`+`movzbl %al,%eax`+`cmp $0x2,%eax`
    // —— 块作用域 register 绑定 %eax 保留二次零扩展（探针 r7 实证；
    //    非 register 或 asm("ebx") 均退化为单条 movzbl/cmp %al）。
    {
    // b28c-l23 P0 清绑：仅去 asm("eax") 绑定，裸 register 保留（值直落、不占槽）。
    int rewardType = pPkt->m_12;
    if (rewardType == 2)
    {
        GlobalData::s_timeGate_->SetLevel(100);
        CUserResetTimeGateQuest modifier;              // ORIG C1 0x084f0886，仅 vptr
        G_GameWorld()->modify_user(&modifier);
        packet.put_short(GlobalData::s_timeGate_->getNpcIndex());
    }
    else
    {
        packet.put_short(0);
    }
    }
    packet.put_int(strlen(pPkt->m_14));
    packet.put_str(pPkt->m_14, strlen(pPkt->m_14));  // ORIG 两次独立 strlen（P48）
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
