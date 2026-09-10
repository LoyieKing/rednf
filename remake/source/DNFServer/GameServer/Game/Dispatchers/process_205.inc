int Dispatcher_WarRoomWpPerMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_WARROOM_WP_PER_MONSTER& mref = reinterpret_cast<MSG_WARROOM_WP_PER_MONSTER&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    // ORIG 0x081d5ca8
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    // ORIG GetWarRoomIndex → movswl（short）→ CGameManager::GetWarRoom(int)
    short warRoomIndex = user->GetWarRoomIndex();
    WarRoom* warRoom = G_CGameManager()->GetWarRoom((int)warRoomIndex);
    if (warRoom == 0)
    {
        return LineFunc(0x3999,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    CUser* target = 0;
    if (mref.m_killerUid != 0xffff)   // ORIG movzwl 0x15; cmp $0xffff
    {
        target = G_GameWorld()->find_from_world(mref.m_killerUid);
        if (target == 0)
        {
            return LineFunc(0x39a4,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
    }

    static_cast<stWarRoomWp&>(param).m_4 = warRoom->HandleWpPerMonster(
        user,
        mref.m_field0d,
        mref.m_field11,
        target);
    return 0;
