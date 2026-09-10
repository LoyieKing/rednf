void CSpecialItemRoutingManager::ProcessFailEtc(CParty* party, int failCode)
{
    if (party != 0)
    {
        // ORIG（idx102 报告）：IsRoutingState 为假则跳过状态读取；5 或 0xc 任一
        // 命中即可路由。布尔全程寄存器（test %al,%al; je），故谓词内联不具名局部
        // （P0 无槽局部）。
        if (party->IsRoutingState() &&
            (party->GetFirstValidMemberState() == 5 ||
             party->GetFirstValidMemberState() == 0xc))
        {
            PacketGuard packet;
            packet.put_header(0, 0x11a);
            packet.put_byte(0);      // ORIG put_byte(local_18,0)
            packet.put_byte(failCode);
            packet.put_int((int)party->GetRoutingItemUniqueID());
            packet.finalize(true);
            party->send_to_party(packet);
            cancelTimer(party);
            party->SetEndRouting();
        }
    }
