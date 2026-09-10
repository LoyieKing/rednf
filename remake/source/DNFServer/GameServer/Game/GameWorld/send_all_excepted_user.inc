void GameWorld::send_all_excepted_user(PacketGuard& packet, CUser* exceptUser)
{
    // ORIG @0x86c8d3x 族：begin sret 直落迭代器槽、无默认 ctor call ⇒ for-init 声明
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() >= 3 && user != exceptUser)
        {
            user->Send(packet);
        }
    }
