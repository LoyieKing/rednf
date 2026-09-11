// ----------------------------------------------------------------------------
// ORIG T 0x08146ba2 _ZN6Global14CBossTowerFunc7MoveMapEP5CUserPt
// state==10 → CDeathTower::handleMoveMap()；state==12 → 虚槽[6] handleMoveMap(
// mapIndex)（CBossStage 新增虚）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
// b28c-L8 判词 [N(N7)]：同 DieCharacInDungeon 的 switch 翻形（ORIG 0x08146ba2 42=42
// 逐条等）；残 6 行 = 虚槽 +0x18 handleMoveMap 的间接 call 目标 %edx vs 4.4 恒 %eax。
bool Global::CBossTowerFunc::MoveMap(CUser* user, unsigned short* mapIndex)  // ORIG T 0x08146ba2
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleMoveMap();
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleMoveMap(mapIndex);  // ORIG vptr+0x18 = 槽[6] handleMoveMap
            }
        }
        return 1;
    }
    return 0;
}