void GameWorld::send_all(PacketGuard& packet, CUser* user)
{
    if (user->IsCurCharacVisible())
    {
        send_all(packet);
    }
    else
    {
        user->Send(packet);
    }
