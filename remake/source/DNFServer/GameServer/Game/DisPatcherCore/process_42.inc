int DisPatcher_GiveUpGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;   // ORIG 不写 param

    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xff7, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }

    // ORIG 每个 state 分支前独立调 get_state（勿缓存）。
    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleLeaveUser(user);
        }
    }
    else if (user->get_state() == 0xc)
    {
        Global::CBossTowerFunc::GiveUpGame(user);
    }
    else if (user->get_state() == 0xd)
    {
        advancealtar::Manager::giveUpGame(user);
    }
    else
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->giveup_game(user, true, false, false);
        }
    }
    return errNo;
