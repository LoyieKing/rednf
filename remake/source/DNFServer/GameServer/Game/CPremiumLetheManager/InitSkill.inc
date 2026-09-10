void CPremiumLetheManager::InitSkill(CUser* user, bool flag,
                                     ENUM_SKILL_TREE_KIND kind)
{
    // [L16 round3] P0/P183：ORIG 段为 `call getCurCharacSkillW; mov 0x14(%ebp),%edx;
    // mov %edx,0x4(%esp); mov %eax,(%esp); call clear_all_skills`（返回值 %eax 直传、
    // 无具名 slot 槽往返）⇒ 删具名缓存、逐点链式重取。
    ((SkillSlot*)user->getCurCharacSkillW())->clear_all_skills(kind);
    ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        int kind2;                       // ORIG：kind==NONE(-1)→0、kind==1(0)→1、其余→2
        if (kind == SKILL_TREE_1)
            kind2 = 1;
        else if (kind == SKILL_TREE_NONE)
            kind2 = 0;
        else
            kind2 = 2;
        user->givePvPSkillTree(0, true, kind2);
        int grade = user->get_pvp_grade();
        int second = (signed char)user->getCurCharSecondGrowType();
        int first = (signed char)user->getCurCharFirstGrowType();
        int job = user->get_charac_job();
        PvPSkillTreeParameterScript* script =
            &G_CDataManager()->m_pvpSkillTree;
        int point = script->getPvPSkillPoint(job, first, second, grade, false);
        ((SkillSlot*)user->getCurCharacSkillW())->set_remain_sp_at_index(point, kind);
        if (flag)
        {
            user->send_skill_info();
            const char* name = user->getCurCharacName();
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0xb1, 0);
            trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                  name);
        }
    }
    else
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x46),
                user->get_charac_job());
        }
        else
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x1de),
                user->get_charac_job());
        }
        std::vector<std::pair<int, int> > vec;
        std::pair<int, int> p;
        char second = user->getCurCharSecondGrowType();
        char first = user->getCurCharFirstGrowType();
        CCharacter* ch = (CCharacter*)(
            *(int*)((char*)G_CDataManager() + 0x14) +
            user->get_charac_job() * 0x7dc);
        char got = ch->get_give_skill(first, second, vec, 0);
        if (got == 1)
        {
            int i;
            for (i = 0; i < (int)vec.size(); ++i)
            {
                p = vec[i];
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), p.first, p.second, kind);
            }
            int expertJob = user->GetCurCharacExpertJobType();
            void* script = G_CDataManager()->GetExpertJobScript(expertJob);
            if (script != 0)
            {
                std::vector<std::pair<int, int> >* expertVec =
                    (std::vector<std::pair<int, int> >*)((char*)script + 0xc);
                unsigned int i2;
                for (i2 = 0; i2 < expertVec->size(); ++i2)
                {
                    int level =
                        user->GetCurExpertJobLevel(
                            user->GetCurCharacExpertJobExp());
                    int skillIdx = (*expertVec)[i2].first;
                    ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                        user->get_charac_job(), skillIdx, level, kind);
                }
            }
            WongWork::CSkillChanger changer;
            int kind3;                  // ORIG：同 kind2（NONE→0、1→1、其余→2）
            if (kind == SKILL_TREE_1)
                kind3 = 1;
            else if (kind == SKILL_TREE_NONE)
                kind3 = 0;
            else
                kind3 = 2;
            changer.SkillInitialize(user, kind3, false);
            if (flag)
            {
                user->send_skill_info();
                const char* name = user->getCurCharacName();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0xf7, 0);
                trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                      name);
            }
        }
    }
