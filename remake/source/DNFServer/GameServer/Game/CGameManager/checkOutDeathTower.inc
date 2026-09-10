void CGameManager::checkOutDeathTower(CUser* user)
{
    // P63 goto 形：checkIn!=1 直跳出口。4.4 对空 then 块穿透（消 nop;jmp），
    // ORIG 4.1 空块 [nop;jmp 出口] 不可复现（跨版本块排布 N，B24D 判词在册）；
    // goto 形消多余尾部中继，为本源码形态最优近似（53/55）。
    WongWork::CDeathTower* tower;
    if ((bool)(user->checkInDeathTower() ^ 1)) goto end;
    tower = getDeathTower(user->getDeathTowerIndex());
    if (tower == 0) goto end;
    // onLeaveUser 返回类型 bool 化后裸 !call() 直产 call;xor$1,%eax;test%al;je（P168a 技巧1）。
    if (!tower->onLeaveUser(user))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x123a,
                              "DeathTower checkOutDeathTower ID[%d] ACCID[%s]",
                              tower->getIdx(),
                              NumberToString(user->get_acc_id(), 0));
    }
end:
    ;
