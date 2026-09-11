// [round5-A20 复核] 同 1 号（ORIG 0x084e8e1e 同构）：fresh .o 251/247；残余同为
//   3~4 处 uchar 比较 eax→edx 中转（N18a/N7，P266 形实测劣化已回退）+ 槽深 N95。
// [K2 2026-09-11 复核] 同 1 号的机理定案（探针 /tmp/k2p/ucmp2.cpp p7 vs p1，
//   /tmp/k2p/ucmp.cpp p1）：根因 = SigTypes_C.h 的 `char m_backupSkill2ND[]` 符号性
//   （0x2b97 系元素与 1 号同族）；**已随 1 号同批落地**（SigTypes_C.h 两字段
//   char→unsigned char + offsetof 锁），4 处比较闭合，余残差同 1 号（store 序 1 条）。
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
            // [AttackB 2026-09-11 真 bug 修正] kind=1 应取 SkillSlot::m_skillSlot2
            // （SkillSlot.h:354 +0x1de）；ORIG 0x084d9b8f / 0x084d9c54 均为
            // `lea 0x1de(%eax),%edx`（%eax=getCurCharacSkillR()），旧形误抄 1 号的
            // m_skillSlot(+0x46) ⇒ 日志写入指向 kind0 技能数组，语义错位。
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot2,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 (unsigned char)sig->m_oneDayLetheFlag2ND == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot2,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
    }
    return false;
}