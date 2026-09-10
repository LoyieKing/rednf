int Inter_LoadEtc::ProcessOneDayLethe(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    // [Lane17 I 判词] ORIG 0x084e8c92：判据⑪ 直方图逐值全等 {−0xc,−0x10,−0x18}。
    // 归一化残差 = 3 处「字节读比较」双段形：ORIG `movzbl 0x2b95(%eax),%edx;
    // movzbl &CPremiumLetheManager::…_STATE,%eax`（2 条）vs 我方 `movzbl 0x2b95(%eax),%eax;
    // mov %eax,%edx; movzbl …,%eax`（3 条，多 eax→edx 中转）⇒ **N18a/N7 封顶**
    // （4.4 对 uchar 比较恒插中转，源码侧 cast/操作数交换均被前端折叠）。
    // 已修：job/skillR 缓存（内联实参）、skill=m_skillSlot(+0x46)、reason=eSkillSaveReason(1)。
    // 残差为 lethe/out/guard 槽深（帧 0x40 vs 0x30）= N82附/N98。
    user->SetOneDayLetheFlag((unsigned char)sig->m_backupSkill[0x198]);  // +0x2b95（backupSkill 尾哨）
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    // ORIG 实证：use = !premium && (f==USE || f==CONFIRM)；flag/use 均无栈槽
    if (!user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
        ((unsigned char)sig->m_backupSkill[0x198] == CPremiumLetheManager::USE_LETHE_STATE ||
         (unsigned char)sig->m_backupSkill[0x198] == CPremiumLetheManager::SKILL_CONFIRM_STATE))
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill), ENUM_SKILL_TREE_KIND(0)))
            return 0x40c6;

        if ((unsigned char)sig->m_backupSkill[0x198] != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag(false);
        }

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x40d2),
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
        out->m_treeKind = 0;
        user->SaveBackupSkillInfo(reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
            out->m_skills), ENUM_SKILL_TREE_KIND(0));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            (unsigned char)sig->m_oneDayLetheFlag == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084d9797 段：无 job/skillR 具名局部
            // （job 经 %ebx 保活）；skill 实参 = SkillSlot::m_skillSlot（`lea 0x46(%eax)`）；
            // 第 4 参 ORIG 立即数 = 1（我方旧写 eSkillSaveReason(0)）。
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 0, eSkillSaveReason(1));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 (unsigned char)sig->m_oneDayLetheFlag == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 0, eSkillSaveReason(1));
        }
    }
    return false;
