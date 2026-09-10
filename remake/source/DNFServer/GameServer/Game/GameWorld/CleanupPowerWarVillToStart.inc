void GameWorld::CleanupPowerWarVillToStart()
{
    // ORIG 0x86cf13e dead init + 0x86cf150 `call size; test; setne; test; je`
    // ⇒ size()!=0 守卫 + 循环外 user 声明
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); ++it)
        {
            user = it->second;
            // ORIG 0x86cf19b `cmp $7,%al`：getCurCharacVill char 返回 QI 直判
            if (user->getCurCharacVill() == 7)
            {
                GlobalData::s_power_manager->ProcessJoinPowerWar(user, 0);
            }
        }
    }
