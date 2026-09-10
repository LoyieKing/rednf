void CLocalChina_User::SendFatigueEvent()
{
    CMaxFatigueFactorEvent* ev =
        static_cast<CMaxFatigueFactorEvent*>(GlobalData::s_event_manager->GetRepeatEvent(2));
    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();
    // [b28d-28] ORIG 0x0812cac5 `jmp <cond>`（while 前置跳转）+ 条件处每轮
    // `call end()`（非缓存）+ 尾部 `ppEi` 后置增量 ⇒ 源形 while(it != end())。
    while (it != G_GameWorld()->m_UserInWorld.end())
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            if (ev != 0)
            {
                _Charac_info* ch = user->getCurCharacW();
                if (ch != 0)
                {
                    if (ev->IsEventing(0))
                    {
                        ch->m_field35 = static_cast<unsigned short>(
                                                        ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100);
                    }
                    else
                    {
                        ch->m_field35 =
                            static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
                    }
                }
                else
                {
                    goto loop_tail;   // ORIG ch==0 跳过 premium/SendFatigue 直入 ++it
                }
            }
            if (user->IsHavePremiumAdvantage())
            {
                bool premiumEventing;
                if (ev == 0)
                {
                    premiumEventing = false;
                }
                else if (!ev->IsEventing(0))
                {
                    premiumEventing = false;
                }
                else
                {
                    premiumEventing = true;
                }
                if (premiumEventing)
                {
                    unsigned short local_e = static_cast<unsigned short>(
                                                static_cast<short>(ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100) -
                        static_cast<short>(DEFAULT_MAX_FATIGUE));
                    unsigned int rate1 = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    int iVar3 = static_cast<int>(rate1 & 0xffff) * DEFAULT_MAX_FATIGUE;
                    unsigned int uVar6 = static_cast<unsigned int>(local_e);
                    unsigned int rate2 = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<short>(iVar3 / 100) +
                        static_cast<short>(static_cast<int>(uVar6 * (rate2 & 0xffff)) / 100)));
                }
                else
                {
                    unsigned int curMax =
                        static_cast<unsigned int>(user->getCurCharacMaxFatigue());
                    unsigned int rate = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<int>((curMax & 0xffff) * (rate & 0xffff)) / 100));
                }
            }
            user->SendFatigue();
        }
loop_tail:
        // [b28d-28] P28 增量侧：ORIG 增量段调 `_Rb_tree_iterator::ppEi`（后置，
        // 带独立 sret 临时槽）⇒ 源形必写 `it++`；`++it` 走 ppEv 免临时槽。
        it++;
    }
