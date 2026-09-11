void GameWorld::modify_user(CUserModifier* modifier)
{
    // ORIG 0x86c9308：begin sret 直落具名 it=-0x18（技巧25 内声明）、user 具名槽 -0xc
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        modifier->Modify(user);
    }
}