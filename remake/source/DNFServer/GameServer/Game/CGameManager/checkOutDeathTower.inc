// [B24D判词 N] 语义正确：checkInDeathTower 以 (bool)(x^1) 判断复现 xor$1；
//   checkIn==1 跳过；else getDeathTower(idx)→onLeaveUser(x^1)→logFormat(0x123a)。
//   diff=句尾合并跳转链我方多一 `jmp;nop` 对（4.1 vs 4.4 对齐），57/55 纯
//   lowering/对齐表示差异，语义等价。
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
                              // [K9GameRestA-S322 · §6.5.1] ORIG 0x0829833a：实串 @0x8c17024
                              // = EUC-KR "없는유저 사망의뢰 아웃처리 ID[%d] ACCID[%s]"。
                              "\xbe\xf8\xb4\xc2\xc0\xaf\xc0\xfa \xbb\xe7\xb8\xc1\xc0\xc7\xc5\xbe \xbe\xc6\xbf\xf4\xc3\xb3\xb8\xae ID[%d] ACCID[%s]",
                              tower->getIdx(),
                              NumberToString(user->get_acc_id(), 0));
    }
end:
    ;
}