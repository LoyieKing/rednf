// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x8631f0c 见 report。
// [b28d-solo-TimerDispatcher] 判词 N7/N82附 封顶：callee-saved 角色轮换（ORIG
// edi=characNo/esi=loginTick/ebx=UID；4.4 恒 esi/ebx/edi）+ 槽 -0x28↔-0x20。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = callee-saved 角色
// edi/esi/ebx 轮换（N7）⇒ 无语义缺口。
bool TimerDBUpdate::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x8631f0c：getUser(p1, 3)（第二实参为常量 3，increId 占位）。
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == 0)
    {
        return 0;
    }
    // UpdateData/GetUID 为 CUser 成员（CUser 继承 CUserCharacInfo），保持 CUser*。
    // P0：ORIG 三个具名槽 -0x24(loginTick)/-0x20(curSec)/-0x1c(elapsedHours)。
    const int loginTick = user->GetLoginTick();
    const int curSec = static_cast<int>(GlobalData::s_systemTime_.getCurSec());
    const int elapsedHours = (curSec - loginTick) / 0xe10;
    // ORIG 08632fd8-0x8633013：范围判定 `> 0 && <= 1` / `> 0xb && <= 0xc`
    // （cmpl $0;jle / cmpl $1;jg），非 `== 1` 单比较形。
    if (elapsedHours > 0 && elapsedHours <= 1)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x23, 0, ENUM_PACKETCLASS_0);
    }
    else if (elapsedHours > 0xb && elapsedHours <= 0xc)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x24, 0, ENUM_PACKETCLASS_0);
    }
    // ORIG 08633043：`cmp; setne %al; test %al,%al; je` ⇒ P225 while+body 形。
    while (user->GetLoginTick() != p2)
    {
        return 0;
    }
    if (p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3)
    {
        return 0;
    }
    user->UpdateData();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)1, get_rand_int(300) + 600,
                                user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}