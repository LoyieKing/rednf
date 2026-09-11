// [b28d-lane2] verdict=DIFF ndiff=289（ORIG 0x084d894c）：寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=101（ORIG 0x84d894c）：N22 cmp 操作数方向+N18a 单段 regmove。
int Inter_UpdateOneDayLethe::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpdateOneDayLethePktView* pPkt = reinterpret_cast<Inter_UpdateOneDayLethePktView*>(data);
    if (pPkt->m_4 ==
        CPremiumLetheManager::USE_LETHE_STATE)
    {
        if (pPkt->m_0 != 0)        // sig+0x0 characNo
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3f8f, 0)("ONE_DAY_LETHE : INTER_USE_LETHE, char(%s)",
                  user->getCurCharacName());

            if (pPkt->m_19e == -1 ||
                pPkt->m_19e == 0)
            {
                user->SetOneDayLetheFlag(
                    CPremiumLetheManager::USE_LETHE_STATE);
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)0);
            }
            else
            {
                user->SetOneDayLetheFlag2ND(
                    CPremiumLetheManager::USE_LETHE_STATE);
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)1);
            }

            PacketGuard packet;                        // -0x3c
            if (user->GetCurCharacSkillTreeIndex() == -1)
            {
                if (pPkt->m_19e == 0)
                {
                    packet.put_header(0, 0xb4);
                    packet.put_byte(2);
                    packet.put_byte(1);
                    packet.finalize(true);
                    user->Send(packet);
                }
            }
            else
            {
                char state = 0;                        // -0x9（ORIG 084d8aeb movb $0x0）
                if (pPkt->m_19e == -1 ||
                    pPkt->m_19e == 0)
                    state = 1;
                else
                    state = 2;
                packet.put_header(0, 0xb4);
                packet.put_byte(2);
                packet.put_byte((int)state);           // movsbl 有符号扩展
                packet.finalize(true);
                user->Send(packet);
            }
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::SKILL_CONFIRM_STATE)
    {
        if (pPkt->m_0 != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3fbd, 0)("ONE_DAY_LETHE : INTER_CONFIRM_SKILL_STATE, char(%s)",
                  user->getCurCharacName());

            WongWork::CHandlePremium::handleRemoveOneDayLethePremium(user);

            if (pPkt->m_19e == -1 ||
                pPkt->m_19e == 0)
            {
                user->SetOneDayLetheFlag(
                    CPremiumLetheManager::NOT_USE_LETHE_STATE);
                pPkt->m_19d =
                    CPremiumLetheManager::SKILL_CONFIRM_STATE;
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)0);
            }
            else
            {
                user->SetOneDayLetheFlag2ND(
                    CPremiumLetheManager::NOT_USE_LETHE_STATE);
                pPkt->m_19d =
                    CPremiumLetheManager::SKILL_CONFIRM_STATE;
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)1);
            }
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::INIT_LETHE_SKILL)
    {
        if (pPkt->m_19e == -1 ||
            pPkt->m_19e == 0)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)0);
        }
        else
        {
            user->SetOneDayLetheFlag2ND(
                CPremiumLetheManager::USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)1);
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::NOT_USE_LETHE_STATE)
    {
        if (pPkt->m_19e == -1 ||
            pPkt->m_19e == 0)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::NOT_USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)0);
        }
        else
        {
            user->SetOneDayLetheFlag2ND(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::NOT_USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)1);
        }
    }
    return 0;
}