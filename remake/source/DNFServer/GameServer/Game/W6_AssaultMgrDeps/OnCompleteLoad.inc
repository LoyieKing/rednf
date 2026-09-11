// ===========================================================================
// OnCompleteLoad —— ORIG T 0x082ef1e2 (0x97)：错误码包 0x94。
// ===========================================================================
bool CAssaultMgr::OnCompleteLoad(CUser* user)   // 恒返 1；Dispatchers 调用点 xor $1 ⇒ bool
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->CompleteLoad(user);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x94),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}