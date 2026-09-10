void CUser::initAllSkill()
{
    // ORIG 0x0868fc46：直接读 getCurCharacR()->m_field7b(+0x7b) 与
    // 0x50c79ed0 比较（setl），不经 getCurCharacLastPlayTick 转发。
    // P0：ORIG threshold 槽双写（0x0868fc5b: movl $0x50076318 后立即
    // movl $0x50c79ed0 覆写同槽）⇒ 源码「声明=旧值 + 覆写」两句式（P197③ 族）。
    int allSkillLimit = (int)0x50076318;
    allSkillLimit = (int)0x50c79ed0;
    if (getCurCharacR()->m_field7b < allSkillLimit)
    {
        ((SkillSlot*)getCurCharacSkillW())->clear_all_skills((ENUM_SKILL_TREE_KIND)0);
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        ((SkillSlot*)getCurCharacSkillW())->InitSkill(false, (ENUM_SKILL_TREE_KIND)0);

        ((SkillSlot*)getCurCharacSkillW())->clear_all_skills((ENUM_SKILL_TREE_KIND)1);
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        ((SkillSlot*)getCurCharacSkillW())->InitSkill(true, (ENUM_SKILL_TREE_KIND)1);

        if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
        {
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (lethe != 0)
            {
                SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);
                lethe->BackupSkill(this, (ENUM_SKILL_TREE_KIND)0);
                SetOneDayLetheFlag2ND(CPremiumLetheManager::NOT_USE_LETHE_STATE);
                lethe->BackupSkill(this, (ENUM_SKILL_TREE_KIND)1);
            }
        }
    }
