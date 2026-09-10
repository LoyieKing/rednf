int Global::CBossTowerFunc::GiveUpGame(CUser* user)
{
    switch (user->get_state())
    {
    case 5:
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->giveup_game(user, true, false, false);
            }
        }
        break;
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleLeaveUser(user);
            }
        }
        break;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleLeaveUser(user);  // ORIG vptr+0x14 = 槽[5]
            }
        }
        break;
    }
    return 1;
