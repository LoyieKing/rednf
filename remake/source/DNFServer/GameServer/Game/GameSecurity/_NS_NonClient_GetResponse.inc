int _NS_NonClient_GetResponse(CUser* user, PacketBuf& packet)
{
    if (user == 0)
        return 0;

    user->SetNonClientFlag(true);
    int randInt;
    if (!packet.get_int(randInt))
        return 1;
    char result;
    if (!packet.get_byte(result))
        return 2;

    switch (result)
    {
        case 0:
        {
            int local = user->GetNonClientRandInt();
            int hash;
            if (!_NS_MakeHash_(reinterpret_cast<char*>(&local), 4, hash))
                return 0;
            if (hash != randInt)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(0x33b), 1, 0, 0);
            }
            break;
        }
        case 1:
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x33d), 1, 0, 0);
            break;
        default:
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x33e), 1, 0, 0);
    }
    return 0;
