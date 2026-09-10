int Dispatcher_UseRightOfChangeGrowType::check_error(CUser* user, MSG_BASE& msg)
{
    if (user != 0 && user->getCurCharacR() != 0)
    {
        return 0xffffffff;
    }
    if (user == 0 || user->get_state() != 3)
    {
        return 0x1e;
    }
    if (user->get_charac_job() == 9)
    {
        return 0xea;
    }
    if (user->get_charac_job() == 10)
    {
        return 0xea;
    }
    if (user->getCurCharFirstGrowType() == 0)
    {
        return 0x13;
    }
    if (user->CheckInTrade() != 0)
    {
        return 0xd8;
    }
    const unsigned char growCount =
        *reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&msg) + 0xf);
    if (static_cast<signed char>(growCount) < 0 || growCount > 5)
    {
        return 1;
    }
    const signed short slotIdx =
        static_cast<signed short>(
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&msg) + 0xd));
    const Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slotIdx));
    CItem* citem = G_CDataManager()->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 0x11;
    }
    UserQuest* quest = user->getCurCharacQuestRefR();
    if (quest->isDoingJobChangeQuest())
    {
        return 0x16;
    }
    if (!user->verifyStackableItemActionPlace((StackableAction::Type)0x2e, citem))
    {
        return 0x17;
    }
    return 0;
