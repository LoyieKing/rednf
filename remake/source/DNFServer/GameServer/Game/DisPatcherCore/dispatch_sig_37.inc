int DIspatcher_OneDayLetheTicket::dispatch_sig(CUser* user, PacketBuf& /*packet*/)

{
    // ORIG 0x0821b106（报告 DIspatcher_OneDayLetheTicket/dispatch_sig.md 全文转写）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0xceaa, 0);
    trace("ONE_DAY_LETHE : SKILL_INIT_PACKET");
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    // ORIG（report Ghidra C）：blocked 判定为**早返**（AL 直测无槽，P198），
    // 随后 GetPremiumLetheManager 空检、ResetSkillReq 各为独立早返——禁布尔局部
    // 合并（合并致 blocked spill 到 -0xd 槽 + 条件合一，与 ORIG 三段独立早返不符）。
    if (G_GameWorld()->IsPVPChannel() && !G_GameWorld()->IsPvPSkilTreeChannel())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    if (!lethe->ResetSkillReq(user))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
    }
    return 0;
}
