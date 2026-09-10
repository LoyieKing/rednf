int CAssaultMgr::OnConnectP2PAfterAssault(CUser* user, char* ip, int len)
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
