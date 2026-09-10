void CUser::RecoverCoin(unsigned int v)
{
    // ORIG 0x8657f10：getCurCharacR 结果 sete 物化 guard（技巧 22 register bool al）；
    // pEvent@-0xc（先赋后测 `cmpl $0,-0xc; je` 短路岛 @8657f46-4a，技巧 40）、
    // IsEventing 尾 `test %al; je F; mov$1;F:` 物化（@8657f66）；三处 GetCoin 结果
    // 直读 %eax `cmp 0xc(%ebp),%eax` 免槽（技巧 37，ORIG 0x8657faa/0x8657fe2/0x8657ff8）。
    if (getCurCharacR() == 0)
        return;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    // P0/N96 去绑定：短路守卫直判（P224 极性族）——具名 eventOn bool 槽反使
    // pEvent 平移 -0xc→-0x10（ORIG eventOn 为 4.1 al 物化无槽，N96 代价）。
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0))
    {
        v = pEvent->GetCoinNoPerDay(get_charac_level());
        // ORIG 0x8657faa：`call GetCoin; cmp 0xc(%ebp),%eax` 后无消费者 =
        // 4.1 发射**空 if 死比较**（P184 死块照抄；probe p15.d1 实证 4.4
        // `if (call() != v) {}` 保留裸 dead cmp 无 setne 展开）。
        if (getCurCharacInvenR()->GetCoin() != v)
        {
        }
    }
    if (IsHavePremiumAdvantage())
    {
        v += m_premium.GetAdvantageCoin();
        if (getCurCharacInvenR()->GetCoin() != v)  // ORIG 0x8657fe2 同形死比较
        {
        }
    }
    if (getCurCharacInvenR()->GetCoin() < v)
        SetOpencoin((char)0);
    else
        SetOpencoin((char)1);
