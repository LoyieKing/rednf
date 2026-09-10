void GameWorld::OnRefreshPowerWarProcessInfo()
{
    // ORIG 0x86cedfb dead init + 0x86cee0d `size(); test; setne; test; je` 守卫
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        user = it->second;
        // ORIG 0x86cee5b `test %al; setne %al; test; je`：uchar 返回单判物化
        if (user->isJoinPowerWar())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            int last = user->GetPowerWarProcessInfoLastUpdateTime();
            // m_powerParameter(+0xa628) + 0x30
            int interval =
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo * 0x3c;
            // ORIG 0x86ceea1-0x86ceeb0 `cmp -0xc,%eax; jle`：差值先算、方向 (now-last)>interval
            if (now - last > interval)
            {
                GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
                    ->SendPowerWarProcessInfo(user->get_charac_no(-1));
            }
        }
    }
    }
