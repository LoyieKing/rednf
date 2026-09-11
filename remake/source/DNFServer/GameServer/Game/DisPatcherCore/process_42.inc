// ORIG 0x081c40b4 T _ZN21DisPatcher_GiveUpGame7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_GiveUpGame/process.md 全文转写）。
// err>0 返 -1；<0 LineFunc(0xff7,pretty,0,acc_id) 返回其值；==0 按 state 分派：
// 10=getDeathTower 非空 handleLeaveUser；0xc=CBossTowerFunc::GiveUpGame；
// 0xd=advancealtar::Manager::giveUpGame；其它=GetParty 非空 giveup_game(user,true,false,false)。
// 尾部返回 check_error 原值（ORIG mov -0x14,%eax）。param 不写。
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
}
