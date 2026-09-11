// ===================== initFairPVPSkillPoint =====================
// [b28d-solo-CDataManagerMethods] IDENTICAL（orig=506 our=506 ndiff=0）。
// 关键修法：删 `PvPSkillTreeParameterScript& s` 引用局部（P0 表达式直用）、
// perIt->second 逐使用点展开（补 5 处 operator-> 调用点）、push_back 改
// `std::pair<int,int>(rem->first, rem->second)` 直构（P192）、两处 continue
// 改正向 if 包裹、三处 cMyTrace 改 P221 全表达式临时链翻槽序。
// ORIG 0x0835f1d6：
// 1) m_pvpSkillTree.m_map0 非空时取内层最大等级 → m_field48(+0xa848)；
// 2) m_pvpSkillTree.m_map18 非空时取外层最大键 → m_field4c(+0xa84c)；
// 3) m_pvpSkillTree.m_map30 非空时逐 FAIR 条目核算：
//    加技能表累计 SP 总和并建 total[skill]=sp；
//    减技能表扣减 total 对应项（负值告警 9081）、回填 giveSkills(+0x34)、
//    累计差值告警 9104；收尾总额复核告警 9135；点数写 m_pointCost(+0x40)。
//    find_skill 经全局 G_CDataManager()（与 ORIG 调用形态一致），
//    参数序 (job, skillId) 为汇编实证（ebx=+0x00 字节, esi=map key）。
bool CDataManager::initFairPVPSkillPoint()
{
    bool ok = true;

    if (!m_pvpSkillTree.m_map0.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      std::map<int, PvPSkillTreeValue> >::iterator it =
                 m_pvpSkillTree.m_map0.begin();
             it != m_pvpSkillTree.m_map0.end(); ++it)
        {
            for (std::map<int, PvPSkillTreeValue>::iterator lv =
                     it->second.begin();
                 lv != it->second.end(); ++lv)
            {
                if (m_pvpSkillTree.m_field48 < lv->first)
                {
                    m_pvpSkillTree.m_field48 = lv->first;
                }
            }
        }
    }

    if (!m_pvpSkillTree.m_map18.empty())
    {
        for (std::map<int, std::map<int, int> >::iterator it =
                 m_pvpSkillTree.m_map18.begin();
             it != m_pvpSkillTree.m_map18.end(); ++it)
        {
            if (m_pvpSkillTree.m_field4c < it->first)
            {
                m_pvpSkillTree.m_field4c = it->first;
            }
        }
    }

    if (!m_pvpSkillTree.m_map30.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator perIt =
                 m_pvpSkillTree.m_map30.begin();
             perIt != m_pvpSkillTree.m_map30.end(); ++perIt)
        {
            std::map<int, int> totalSp;
            int sum = 0;
            for (std::map<int, int>::iterator add =
                     perIt->second.m_addSkills.begin();
                 add != perIt->second.m_addSkills.end(); ++add)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, add->first);
                if (skill != 0)
                {
                    int sp = skill->get_need_sp_for_level(0, add->second);
                    sum += sp;
                    totalSp.insert(std::make_pair(add->first, sp));
                }
            }

            int running = 0;
            for (std::map<int, int>::iterator rem =
                     perIt->second.m_removeSkills.begin();
                 rem != perIt->second.m_removeSkills.end(); ++rem)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, rem->first);
                if (skill != 0) {
                int sp = skill->get_need_sp_for_level(0, rem->second);
                sum -= sp;

                std::map<int, int>::iterator found = totalSp.find(rem->first);
                if (found != totalSp.end())
                {
                    found->second -= sp;
                    if (found->second < 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x2379, 0)
                            ("skill point (-) error");
                        ok = false;
                    }
                }

                perIt->second.m_giveSkills.push_back(
                    std::pair<int, int>(rem->first, rem->second));

                int current = 0;
                for (std::map<int, int>::iterator it = totalSp.begin();
                     it != totalSp.end(); ++it)
                {
                    current += it->second;
                }
                if (current + running != sum)
                {
                    running += sum - current;
                    cMyTrace(__PRETTY_FUNCTION__, 0x2390, 0)
                        ("skill point diff job(%d), skill(%d)",
                         (int)perIt->first.m_a, rem->first);
                    ok = false;
                }
                }
            }

            int grand = 0;
            for (std::map<int, int>::iterator it = totalSp.begin();
                 it != totalSp.end(); ++it)
            {
                grand += it->second;
            }
            if (grand != sum)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x239f, 0)
                    ("skill point diff");
                ok = false;
            }

            perIt->second.m_pointCost = sum;
        }
    }

    return ok ? 1 : 0;
}