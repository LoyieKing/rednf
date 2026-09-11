bool Timer_PcRoomAuthRetry::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
    {
        return 0;
    }
    // ORIG +0x38/+0x40: get_unique_id() 低 16 位与 p2 比较
    if ((unsigned short)user->get_unique_id() != p2)
    {
        return 0;
    }
    // ORIG +0x57: getCurCharacR() 判空
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG +0x72: getCurCharacNo() 与 p3 比较
    if (user->getCurCharacNo() != p3)
    {
        return 0;
    }
    // ORIG +0x8e..+0xa5: !getPcRoomAuth()（bool 声明 ⇒ xor $0x1,%eax 形态）
    if (!user->getPcRoomAuth())
    {
        user->SendLoginToPCRoomServer();
    }
    return 1;
}