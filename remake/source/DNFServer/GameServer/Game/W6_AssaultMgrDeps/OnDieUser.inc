int CAssaultMgr::OnDieUser(CUser* user, int index, MSG_DIE_ASSAULT_PLAYER& msg)
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->DieUser(user, index, msg);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x96),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
