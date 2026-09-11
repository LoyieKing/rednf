// ===========================================================================
// OnDieUser —— ORIG T 0x082ef3be (0xa5)：错误码包 0x96。
// ===========================================================================
bool CAssaultMgr::OnDieUser(CUser* user, int index, MSG_DIE_ASSAULT_PLAYER& msg)   // [P354/P292] ORIG xor $1 ⇒ bool
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
}