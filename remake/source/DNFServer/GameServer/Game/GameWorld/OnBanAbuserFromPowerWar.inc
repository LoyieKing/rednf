// [b28d-11b] N：局部/槽位差（N82）@0x086cef36
// [b28d-solo-GameWorld] _ZN9GameWorld23OnBanAbuserFromPowerWarEv verdict=AE 140=140
// 修法：P197③ 两段式预清 `CUser* user = NULL;` + `if (m_UserInWorld.size() != 0)` 守卫
// （复现 ORIG 0x086cef3e movl $0,-0x18 + 0x086cef50 call size; test; setne; test; je）
// + P221 全表达式临时链 cMyTrace(...)(...)（具名 trace 槽 -0x2c vs ORIG -0x3c；130→139→140）。
void GameWorld::OnBanAbuserFromPowerWar()
{
    // [b28d-solo-GameWorld] P197③ 两段式预清 + size()!=0 守卫：ORIG 0x086cef3e
    // `movl $0,-0x18` + 0x086cef50 `call size; test %eax,%eax; setne %al; test; je`。
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        user = it->second;
        if (user->isJoinPowerWar())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            int last = user->GetPowerWarLastPlayTime();
            // m_powerParameter(+0xa628) + 0x34
            int limit = G_CDataManager()->m_powerParameter.m_abuserBanTime * 0x3c;
            if (limit < now - last)
            {
                GlobalData::s_power_manager->ProcessJoinPowerWar(user, 0);
                PacketGuard packet;
                packet.put_header(0, 0xe2);
                packet.finalize(true);
                user->Send(packet);
                // [b28d-solo-GameWorld] P221：全表达式临时链复现 ORIG trace 深槽 -0x3c
                // （具名 trace 落 -0x2c，迭代器槽随之浅移）。
                cMyTrace(__PRETTY_FUNCTION__, 0x178e, 0)(
                      "[PowerWar] POWER WAR BAN USER (USER:%s(%d), LAST:%d, CURR:%d)",
                      user->getCurCharacName(), user->getCurCharacNo(), last, now);
            }
        }
    }
    }
}