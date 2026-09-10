bool CUser::_CheckCompoundSkill(const CStackableItem* item, bool checkRecipe)
{
    static const char* PRETTY =
        "bool CUser::_CheckCompoundSkill(const CStackableItem*, bool)";

    if (item->m_recipe174.m_field24 != 0 &&
        isAffectedPremium((ENUM_PREMIUM_TYPE)7) == false)
    {
        return false;
    }

    if (item->m_recipe174.m_field25 != 0)
    {
        // [b28d-solo-CUserOnlyOrig P167] ORIG 无 C1ERKS_/D1Ev 差集 ⇒ 实参是匿名右值
        // 临时（直构进按值参数槽），非具名 list 局部（具名左值恒调拷贝构造 + 析构）。
        bool ok = reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())->check_skill_list(
            get_charac_job(),
            std::vector<std::pair<int, int> >(item->m_recipe174.m_vec18), 2);
        if (ok)
        {
            return expert_job::CExpertJob::IsLearnRecipe(this, item);  // ==1 → true
        }
        unsigned int characNo = getCurCharacNo();
        cMyTrace trace(PRETTY, 0x5c68, 0);
        trace("EXPERT_JOB_ERROR : check_skill_list() charac_no(%d)", characNo);
        return false;
    }

    if (!checkRecipe)
    {
        bool ok = reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())->check_skill_list(
            get_charac_job(),
            std::vector<std::pair<int, int> >(item->m_recipe174.m_vec18), 2);
        if (!ok)
            return false;
    }
    return true;
