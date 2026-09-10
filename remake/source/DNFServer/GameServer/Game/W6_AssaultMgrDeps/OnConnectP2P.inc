int CAssaultMgr::OnConnectP2P(CUser* user, char* ip, int len)
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->ConnectP2P(user, ip, len);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x95),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
