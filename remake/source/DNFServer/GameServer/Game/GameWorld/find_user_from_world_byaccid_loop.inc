CUser* GameWorld::find_user_from_world_byaccid_loop(unsigned int accId)
{
    // ORIG 0x86c4cb0-0x86c4d03：begin sret 直落 -0x10、++it 前缀 opEv（无 ppEi temp）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        if (it->second->get_acc_id() == accId)
        {
            return it->second;
        }
    }
    return NULL;
