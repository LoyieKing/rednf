// ===========================================================================
// _SendVictoryInfo —— ORIG T 0x082efcb2 (0x161)
// 包 header(0,0x10a)；PowerSide==1 时 [胜方连数 A][B]，否则反向；
// finalize(true) 后两队分别 send_to_party 或单人 Send。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（105=105）：单 party 槽 -0xc 复用两处 GetParty + P297 反条件
// `je <Send>` 布局；旧 N93 判词作废。ORIG 0x082efcb2。
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
}