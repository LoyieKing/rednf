// ===========================================================================
// OnConnectP2PAfterAssault —— ORIG T 0x082ef320 (0x9d)
// 会战结束后阶段：P2P 直连改经队伍转发；失败错误码包 0xf9。
// ===========================================================================
bool CAssaultMgr::OnConnectP2PAfterAssault(CUser* user, char* ip, int len)
{
    try
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->ConnectP2P(user, ip, len);
        }
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf9),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}