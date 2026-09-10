void GiveGrowCreatureEvent::StartAction()               // 0x081b6abc
{
    // 遍历全服在线用户，state>2（地下城中在线态）广播事件信息。
    // [b28d-28] P183/P13/P28：ORIG 每轮重调 G_GameWorld()（begin/end 各自
    // 重物化，无 world/users 缓存局部）+ 增量段 `ppEi` 后置 ⇒ 源形如下。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            SendGrowthCreatureEventInfo(*user);
        }
    }
