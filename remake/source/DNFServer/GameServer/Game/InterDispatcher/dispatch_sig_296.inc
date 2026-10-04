// [b28d-lane2] verdict=DIFF ndiff=79（ORIG 0x084e7598）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=101（ORIG 0x84e7598）：P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_CollectItemsRewardBroadcast::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [N 判词·2026-09-11 复核] ORIG pPkt@-0xc / packet@-0x18 vs 我方 -0x10/-0x1c（整体深 4）。
    //   试形（Main「指针局部→整型承载地址」旋钮）实测 **反劣**（our 103→104、gap −1→−2）
    //   ⇒ 回退，落 N82/P212 槽方向。
    Inter_CollectItemsRewardBroadcastPktView* pPkt = reinterpret_cast<Inter_CollectItemsRewardBroadcastPktView*>(data);

    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(0xb);
    packet.put_byte((unsigned int)pPkt->m_rewardType);
    // ORIG +0x60..+0x67：`movzbl 0x12(%eax),%eax`+`movzbl %al,%eax`+`cmp $0x2,%eax`
    // —— 块作用域 register 绑定 %eax 保留二次零扩展（探针 r7 实证；
    //    非 register 或 asm("ebx") 均退化为单条 movzbl/cmp %al）。
    {
    // b28c-l23 P0 清绑：仅去 asm("eax") 绑定，裸 register 保留（值直落、不占槽）。
    int rewardType = pPkt->m_rewardType;
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
    packet.put_int(strlen(pPkt->m_name));
    packet.put_str(pPkt->m_name, strlen(pPkt->m_name));  // ORIG 两次独立 strlen（P48）
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}