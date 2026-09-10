void CEventManager::BroadcastEventInfo()
{
    // b28d-solo-CEventManager: [AE] ORIG 0x08116108 实证——PacketGuard 构造在循环外一次
    // （-0x20 槽，循环内复用）；无 world==0 / user==0 防御（P140/P171 照抄）；
    // begin 一次求值、end 每轮经 G_GameWorld() 重求值；迭代器后置 `it++`（P28，
    // ORIG 增量段 ppEi）。80=80 逐条归一化后仅剩 call/全局地址重定位伪差（P264）。
    PacketGuard packet;
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            MakeNotiEventInfo(packet, user);
            user->Send(packet);
        }
    }
