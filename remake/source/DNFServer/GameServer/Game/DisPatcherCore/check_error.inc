unsigned int DisPatcher_SelectCharac::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedCreateCharac() != false)
    {
        return 0x13;
    }
    if (user->isHumanCertified() != true)
    {
        return 0x7fffffff;
    }
    MSG_CHARAC_SELECT& sel = reinterpret_cast<MSG_CHARAC_SELECT&>(msg);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        unsigned int uVar6 = user->getOnlinePreliminaryCharacNo();
        sel.m_slot = static_cast<signed char>(user->get_charac_idx(uVar6));
        if (static_cast<unsigned char>(sel.m_slot) == 0xff)
        {
            return 0xffffffff;
        }
    }
    if (checkPvpEnterCharacter(user, sel) != true)
    {
        return 0xffffffff;
    }
    if (('#' < static_cast<char>(sel.m_slot)) || (static_cast<char>(sel.m_slot) < '\0'))
    {
        signed char MVar1 = sel.m_slot;
        cMyTrace local_34(__PRETTY_FUNCTION__, 0x16c4, 5);
        local_34("pUser->SelectCharac() fail, %d out of range", static_cast<int>(MVar1));
        return 4;
    }
    if (user->m_characList.size() <= static_cast<unsigned char>(sel.m_slot))
    {
        return 0xfffffffe;
    }
    std::string local_38(user->m_characList[static_cast<int>(sel.m_slot)].m_name);
    bool bVar4;
    if (CSyncSlangFilter::GetInstance()->HasSlangName(local_38) == false &&
        CSyncSlangFilter::GetInstance()->HasSlang(local_38) == false)
    {
        bVar4 = false;
    }
    else
    {
        bVar4 = true;
    }
    int uVar10;
    if (bVar4)
    {
        uVar10 = 0x9f;
    }
    else
    {
        unsigned int uVar6 = user->get_acc_id();
        bool bVar4b = GlobalData::s_pGMAccounts_->isGM(uVar6);
        G_CDataManager()->setCharacNameFilterPass(bVar4b);
        if (G_CDataManager()->hasPreventPrefixCharacName(
                user->m_characList[static_cast<int>(sel.m_slot)].m_name,
                static_cast<int>(strlen(user->m_characList[static_cast<int>(sel.m_slot)].m_name))) != false)
        {
            uVar10 = 8;
        }
        else if (user->checkSelectCharac(static_cast<int>(sel.m_slot)) == true)
        {
            if (user->isLockedDeleteCharac() == false)
            {
                if (user->lock4DataLoad() == true)
                {
                    uVar10 = 0;
                }
                else
                {
                    uVar10 = 0xfffffffc;
                }
            }
            else
            {
                int uVar9 = user->m_characList[static_cast<int>(sel.m_slot)].m_characNo;
                cMyTrace local_20(__PRETTY_FUNCTION__,
                                  0x171c, 0);
                local_20("select_charac error during delete charac_no(%d)", uVar9);
                uVar10 = 0x7fffffff;
            }
        }
        else
        {
            uVar10 = 0xfffffffd;
        }
    }
    return uVar10;
