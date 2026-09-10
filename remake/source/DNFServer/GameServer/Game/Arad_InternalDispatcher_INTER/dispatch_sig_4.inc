int Arad_INTER_kAradJumpingCharacter::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER] 真 bug 修复：sig 原取自 len（0x14(%ebp) int），ORIG 0x08199a28 读
    // 0x10(%ebp)=body(char*)；字段访问改 arad::SigAradJumpingCharacter 具名成员
    // （ORIG movzwl 0xb(%eax)/movzbl 0x9(%eax)/0xa(%eax) disp 折叠，裸偏移 sig+N
    // 恒产 add $N,%eax + movzwl (%eax) 两步，§4.3）。
    if (user == 0)
        return 0x5e9;
    if (user->getCurCharacR() == 0)
        return 0x5ee;

    arad::SigAradJumpingCharacter* sig = (arad::SigAradJumpingCharacter*)body;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x5f3, "JUMPING_CHARACTER INTER_DSP(SigAradJumpingCharacter) null.");
        return 0x5f4;
    }

    // 跳级券：delete_item(INVEN 1, slot=sig->m_itemIndex, 1, reason 3, true)
    // [b28d-solo] P157 逐字段重读：ORIG 两次独立 movzwl 0xb(%eax)（delete_item 前
    // %ebx 保活、SendUpdateItemList 处重读），禁具名 slot/inventory 缓存。
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, sig->m_itemIndex, 1,
                                                (eItemDelReason)3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x13);
        return 0;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, sig->m_itemIndex);

    // 升到 19→20 区间（<=0x13 循环自增）。
    while (user->get_charac_level() <= 0x13)
        user->SetCharacLevel(user->get_charac_level() + 1);

    // 一次成长（growValue=m_growType），二段成长（m_eventKind==2 时两次跳段）。
    int firstGrowTarget = (int)sig->m_growType;             // -0x14
    int secondGrowFlag = 0;                                 // -0x10
    user->ChangeGrowType_GM(firstGrowTarget, secondGrowFlag);
    if (sig->m_eventKind == 2)
    {
        while (user->get_charac_level() <= 0x31)
            user->SetCharacLevel(user->get_charac_level() + 1);
        secondGrowFlag = 1;
        // ORIG 0x08199bab `call getCurCharFirstGrowType; movsbl %al,%eax` 直传，
        // 无具名 curFirstGrow 槽（具名形使 sig 起全帧槽基深 4）。
        user->ChangeGrowType_GM((int)user->getCurCharFirstGrowType(), secondGrowFlag);
        while (user->get_charac_level() <= 0x36)
            user->SetCharacLevel(user->get_charac_level() + 1);
    }

    user->send_skill_info();
    clearEpicQuest(user);

    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info((char*)&packet);
    user->Send(packet);
    user->send_clear_quest_list();
    user->sendCharacQp();

    int exp = G_CDataManager()->get_level_exp(user->get_charac_level());
    user->setCurCharacExp(exp);

    sendHelperBox(user, (unsigned char)sig->m_eventKind, sig->m_growType);

    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
    user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    return 0;
