int Inter_LoadEtc::ProcessOneDayLethe2ND(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    // [Lane17 I 判词] 同 1 号（ORIG 0x084e8e1e 同构）：判据⑪ 直方图逐值全等；
    // 残差同为 3 处 uchar 比较的 eax→edx 中转（N18a/N7）⇒ 封顶。
    // p167 CLEAR 35/35；去缓存 + m_skillSlot + reason=1 已落地。
    user->SetOneDayLetheFlag2ND((unsigned char)sig->m_backupSkill2ND[0x198]);  // +0x2d2f
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    // ORIG 实证同 1 号：use = !premium && (f==USE || f==CONFIRM)；无 flag/use 槽
    if (!user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
        ((unsigned char)sig->m_backupSkill2ND[0x198] == CPremiumLetheManager::USE_LETHE_STATE ||
         (unsigned char)sig->m_backupSkill2ND[0x198] == CPremiumLetheManager::SKILL_CONFIRM_STATE))
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill2ND), ENUM_SKILL_TREE_KIND(1)))
            return 0x4111;

        if ((unsigned char)sig->m_backupSkill2ND[0x198] != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag2ND(false);
        }

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x411d),
                           true);
        // [b28d-63] ORIG 0x84d9602/0x84d962a：`guard << x` 实为 (*guard) << x
        // （每处先 call CStreamGuard::operator*），GetInBuffer 才走 operator->（ptEv）。
        (*guard)->operator<<(0x15c);
        (*guard)->operator<<((int)user->GetUID());
        SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
            guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();  // ORIG 0x84d9641 ptEv
        out->m_accID = user->get_acc_id();
        out->m_characNo = user->getCurCharacNo();
        memset(out->m_skills, 0, 0x198);
        out->m_state2 = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_state = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_treeKind = 1;
        user->SaveBackupSkillInfo(reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
            out->m_skills), ENUM_SKILL_TREE_KIND(1));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            (unsigned char)sig->m_oneDayLetheFlag2ND == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            // [b28d-solo-Inter_LoadEtc_impl] 同 1 号：内联实参 + m_skillSlot(+0x46) + reason=1。
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 (unsigned char)sig->m_oneDayLetheFlag2ND == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
    }
    return false;
