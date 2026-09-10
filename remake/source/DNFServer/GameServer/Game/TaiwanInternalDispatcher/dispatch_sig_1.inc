int InterSelectMobileAuthReward::dispatch_sig(CUser* user, char* buf, int len)
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x161, "[Taiwan, MobileAuth] interMsg is null.");
        return 0x162;
    }
    // ORIG 0x08161384 = 无条件 jmp（源码条件第二项编译期恒假），其后 0x08161389..0x0816158c
    // 的发邮件主体是 ORIG **文本发射的不可达代码**（0x204 字节）。4.4 前端把 `&& 0` 整体
    // 折叠并 DCE 该块 ⇒ 用恒 0 的局部 volatile 守卫迫使后端照常发射（局部量不产生符号；
    // 守卫恒 0 ⇒ 可观察行为与 ORIG 一致：该块永不执行）。
    volatile bool dead = false;
    if ((msg->m_rewarded ^ 1) != 0)
    {
        if (dead)
        {
            std::vector<std::pair<int, int> > items;
            items.push_back(std::pair<int, int>(0x28983a, 1));
            char title[0x1d];
            char mail[0xff];
            strncpy(title, g_scriptStringManager_.findString(4, "tw_mobile_authentication_title_01", 0), 0x1d);
            strncpy(mail, g_scriptStringManager_.findString(4, "tw_mobile_authentication_mail_01", 0), 0xff);
            Inven_Item invenItems[10];
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, invenItems, 10);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                title, invenItems, (unsigned int)items.size(), 0,
                (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0,
                user->GetServerGroup(), false);
            CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xa1);
            if (ev != 0 && ev->IsEventing(0))
            {
                static_cast<CEventCeraShopRewardPoint*>(ev)->SendReward(user);
            }
        }
    }
    user->m_field8ec31 = msg->m_rewarded;
    return 0;
