int TimerFinishItemEffect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0x3);
    if (user == NULL)
        return 0;
    if (user->get_state() <= 2)          // ORIG setle: state<=2 → return 0
        return 0;
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;

    const unsigned int curSec =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
    const std::vector<ContinuousItemInfo>* items =
        user->GetCurCharacEffectItemListR();
    if (items == NULL)
        return 0;

    // 循环条件每次迭代重新取 size()（ORIG +0x290 seta 无符号比较）。
    // [b28d-solo-TimerDispatcher] P157：p167 差集 MISSING = const operator[] ×2
    // （ORIG 对 m_itemIdx/m_expireTime 每次重发 `(*items)[i]`，无 entry 引用缓存）。
    for (unsigned int i = 0; i < static_cast<unsigned int>(items->size()); ++i)
    {
        if ((*items)[i].m_itemIdx != p2)       // 未命中该项：i++ 继续
            continue;

        PacketGuard packet;              // ORIG try 区 0x8637bb1..0x8637cfd
        packet.put_header(0, 0xae);
        packet.put_byte(1);
        packet.put_int(p2);
        packet.put_int(0);
        packet.finalize(true);

        if ((*items)[i].m_expireTime <= curSec)
        {
            // 已到期：立即结算并返回 Send 结果（ORIG esi=0 直通尾部返回 ebx）。
            _processSepecialEffectContinuousItem(user, p2);
            return user->Send(packet);
        }

        const unsigned int remain = (*items)[i].m_expireTime - curSec;
        if (!((remain < 6u) && (0x3cu < remain)))
        {
            // ORIG +0x1d2 重排程：OBJ_TYPE 0 / key=GetUID / msg 0x76 / delay=remain / p2 / characNo
            // [SweepTimerDispatcher2] P329/P0：ORIG 无 uid/characNo 具名槽——characNo 先求值
            // 驻 %edi、uid 驻 %esi（实参右→左求值），我方具名局部使全帧槽深 +8（-0x2c→-0x34）。
            // 改内联实参后槽序应回 ORIG 形。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                        static_cast<int>(user->GetUID()),
                                        (TIMER_MESSAGE)0x76,
                                        static_cast<int>(remain),
                                        p2,
                                        static_cast<unsigned int>(user->getCurCharacNo()));
            continue;                    // esi=1 → 回到 i++ 尾部继续遍历
        }

        // ORIG +0x227 死分支（remain<6 且 >60 同时成立，恒不可达，按汇编原样保留）。
        _processSepecialEffectContinuousItem(user, p2);
        return user->Send(packet);
    }
    return 1;                            // ORIG 遍历结束 ebx=1
