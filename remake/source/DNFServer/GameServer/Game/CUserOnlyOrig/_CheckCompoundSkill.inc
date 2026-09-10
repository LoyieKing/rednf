bool CUser::_CheckCompoundSkill(const CStackableItem* item, bool checkRecipe)
{
    static const char* PRETTY =
        "bool CUser::_CheckCompoundSkill(const CStackableItem*, bool)";

    // [G6Sub1 单旋钮·P225 推广/P133] ORIG 0x08670f2c..0x08670f59 为**两跳同落**形：
    // `test %al,%al; je <主体>` + `call isAffectedPremium; xor $0x1,%eax; test %al,%al;
    // je <主体>` + fall-through `mov $0x0,%eax; jmp 尾`（值无栈槽往返，P0）。
    // 旧 `if (A && B)` 合并短路被 4.4 物化成 bool 岛：多出 `mov $1,%eax; jmp;
    // mov $0,%eax; test %al,%al; je; mov $0,%eax; jmp`（4 条，即本件 gap 主项）。
    // 拆成嵌套 if（各条件独立早退、共享同一失败出口）即逐条复现 ORIG 双跳。
    if (item->m_recipe174.m_field24 != 0)
    {
        if (isAffectedPremium((ENUM_PREMIUM_TYPE)7) == false)
        {
            return false;
        }
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
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 152→152 中性 ⇒ 维持具名形。
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
