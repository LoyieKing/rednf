int Dispatcher_ChangeHostWarRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err;
    short roomIdx;
    CGameManager* mgr;
    WarRoom* room;
    int result;

    err = check_error(user, msg);
    if (err > 0)
        return err;
    if (err < 0)
        return -1;
    roomIdx = (short)user->GetWarRoomIndex();
    mgr = G_CGameManager();
    room = mgr->GetWarRoom(roomIdx);
    if (room == 0)
    {
        LineFunc(0x41a7,
                 __PRETTY_FUNCTION__,
                 0, user->get_acc_id());
    }
    else
    {
        result = room->HostChangeWarRoom(
            user,
            *(reinterpret_cast<char*>(&msg) + 0xd),
            *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(&msg) + 0xe));
        *(reinterpret_cast<int*>(&param) + 1) = result;
    }
    return 0;
