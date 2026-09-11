void GameWorld::send_server_group(PacketGuard& packet, char group)
{
    // ORIG 0x86c90ba-0x86c90d5：begin sret 直落 -0x18 单槽，无默认 ctor call
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() >= 3 &&
            (int)user->GetServerGroup() == (int)group)
        {
            user->Send(packet);
        }
    }
}