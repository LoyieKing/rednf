// ----------------------------------------------------------------------------
// ORIG T 0x08146ad8 _ZN6Global14CBossTowerFunc10GiveUpGameEP5CUser
// state==10 → CDeathTower::handleLeaveUser；state==12 → 虚槽[5] handleLeaveUser；
// state==5 且队伍存在 → giveup_game(user,true,false,false)；恒返回 1。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（59 vs 57）（ORIG 0x08146ad8 sz 0x000000ca）
// ORIG 尾 `mov $0x1,%eax; leave; ret`（单出口返 1）；case 体依源码序 5/0xa/0xc
// 落槽 -0x14/-0x10/-0xc（ORIG 深→浅 = 4.1「先声明更深」）。
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
}