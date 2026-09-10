bool CAssaultMgr::OnChangeHp(CUser* user, int hp)   // 返回类型按 Dispatcher_ChangeHp 调用点 xor $1 证据改 bool（广播 2026-09-05 DispB）
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->ChangeHp(user, hp);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x98),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
