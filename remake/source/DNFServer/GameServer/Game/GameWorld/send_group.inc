// CBelong 唯一声明点=CSchoolMgr.h（b26 NL4 上移；ORIG send_group 0x86c93d 调
// vtable 槽0，`call *%edx; test %al,%al; je`）。
void GameWorld::send_group(PacketGuard& packet, CBelong* belong)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2 && belong->IsRight(user))
        {
            user->Send(packet);
        }
    }
}