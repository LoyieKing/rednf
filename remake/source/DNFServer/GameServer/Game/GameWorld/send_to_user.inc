void GameWorld::send_to_user(int uniqueId, PacketGuard& packet)
{
    CUser* user = find_from_world((unsigned short)uniqueId);
    if (user != NULL)
    {
        user->Send(packet);
    }
