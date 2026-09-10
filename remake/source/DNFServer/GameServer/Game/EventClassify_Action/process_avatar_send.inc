int CEventActionMng::process_avatar_send(CUser* user, Inven_Item& item,
                                         CItem const* citem)
{
    char ipgBuf[24];
    int slot = -1;
    slot = user->getCurCharacInvenW()->AddAvatarItem(
        item.m_addInfo, citem->getUsablePeriod(),
        /*b*/ false, /*c*/ 0, /*d*/ (char)-1,
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
            (unsigned int)user->getCurCharacNo(), ipgBuf),
        (eAvatarItemAddReason)2, /*e*/ false, /*f*/ 0);

    if (slot != -1)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        item.m_addInfo2 =                                   // +0x07
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                user->getCurCharacInvenR()->GetInvenSlot(2, slot).m_addInfo2,
                now);
        item.SetUpgrade(0);
    }
    return slot;
