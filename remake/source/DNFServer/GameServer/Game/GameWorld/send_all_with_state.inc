void GameWorld::send_all_with_state(PacketGuard& packet, int state)
{
    // ORIG：同上，for-init 声明形（无 ctor call/无拷贝）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (state <= user->get_state())
        {
            user->Send(packet);
        }
    }
}