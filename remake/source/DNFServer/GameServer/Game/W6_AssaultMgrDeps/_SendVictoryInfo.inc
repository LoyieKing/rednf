void CAssaultMgr::_SendVictoryInfo(CUser* userA, CUser* userB)
{
    PacketGuard packet;
    packet.put_header(0, 0x10a);
    if (userA->getPowerSide() == 1)
    {
        packet.put_int(userA->GetPowerWarPartyVictory());
        packet.put_int(userB->GetPowerWarPartyVictory());
    }
    else
    {
        packet.put_int(userB->GetPowerWarPartyVictory());
        packet.put_int(userA->GetPowerWarPartyVictory());
    }
    packet.finalize(true);

    // [b28d-solo] ORIG 单槽 -0xc 复用两处 GetParty（0x082efd7c/0x082efdb6）
    // + 反条件 `je <Send>` 布局（party!=0 直落 send_to_party）⇒ P297 反写。
    CParty* party = static_cast<CParty*>(userA->GetParty());
    if (party != 0)
    {
        party->send_to_party(packet);
    }
    else
    {
        userA->Send(packet);
    }
    party = static_cast<CParty*>(userB->GetParty());
    if (party != 0)
    {
        party->send_to_party(packet);
    }
    else
    {
        userB->Send(packet);
    }
