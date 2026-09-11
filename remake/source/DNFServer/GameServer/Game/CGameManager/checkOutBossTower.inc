void CGameManager::checkOutBossTower(CUser* user)
{
    // P63 goto 形（同 checkOutDeathTower）：空 then 块 4.4 穿透，N 封顶；
    WongWork::CBossTower* tower;
    if ((bool)(user->checkInBossTower() ^ 1)) goto end;
    tower = getBossTower(user->getBossTowerIndex());
    if (tower == 0) goto end;
    {
            // **虚槽仲裁（多点联立，非单点）**：ORIG 此处是
            // `mov (%eax),%eax; add $0x14,%eax; mov (%eax),%edx; call *%edx`。
            // %eax 取到的是 vptr（指向 _ZTV+0x08），故 +0x14 ⇒ _ZTV+0x1c。
            // dump `_ZTVN8WongWork10CBossTowerE`@0x08b70260 全槽实测：
            //   +0x00 generateRewardItem  +0x04 generateRewardExp
            //   +0x08 _checkMemberDie     +0x0c checkAllMemberReady(0x081493fe)
            //   +0x10 handleDieUser       **+0x14 handleLeaveUser(0x08143ae2)**
            //   +0x18 handleMoveMap       +0x1c CBossStage::handlePickupItem
            // ⇒ 原注释「+0x14 = checkAllMemberReady」把 **vtable 数组下标**当成
            // **vptr 偏移**，实际被调的是 **CBossTower::handleLeaveUser(CUser*)**。
            // 头文件虚声明顺序本身与 ORIG 完全一致，无需改布局（零 ABI 风险）。
            // X1 形（probe /tmp/xg2.cpp 形 B）：裸 (bool)(x^1) 产 cmp$1;setne，
            // N96/P0：bool callee 裸形（P168a①）直产 call;xor$1;test;je；
            // 残差 call *%eax vs ORIG *%edx 属 N7 寄存器名族。
            if (!tower->handleLeaveUser(user))
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x11d9,
                                      "BossTower checkOutBossTower ID[%d] ACCID[%s]",
                                      tower->getIdx(),
                                      NumberToString(user->get_acc_id(), 0));
            }
        }
end:
    ;
}