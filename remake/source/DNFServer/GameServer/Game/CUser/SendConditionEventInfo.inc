void CUser::SendConditionEventInfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xea);
    CConditionEventManager* pMgr = G_CGameManager()->GetConditionEventManager();
    if (pMgr)
    {
        int eventIdx = pMgr->GetCurEventIndex();
        // ORIG 0x868195d/0x868196b：`mov %al,-0xa/-0x9`（char 槽直存）+ movsbl 读
        // ——局部为 char（signed byte），put_byte 传参符号扩展。
        char step = (char)GetCurConditionEventStep();
        char rewardStep = (char)GetCurConditionEventRewardStep();
        packet.put_short(eventIdx);
        packet.put_byte(step);
        packet.put_byte(rewardStep);
        packet.finalize(true);
        Send(packet);
    }
}