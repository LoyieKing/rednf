void CSkillList::set_post_learning_skill(int jobType, CSkill* skill)
{
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x08351e62/0x08351e94：find/insert
    // 的 map key 取 `it->+0x00`（ptEv 结果直传，_pre_skill::m_index），旧实现误用
    // m_jobType(+0x04)（get_post_learning_skill 亦以 skillIdx 为 key 互证）。
    // value 为 pair<int,int> C1Ev 后逐字段赋值（-0x38/-0x34）；&& 拆嵌套 if 复现
    // 4.1 的 `setne %al; test` 第二条件物化（P225 推广）。
    if (skill != 0)
    {
        if (skill->m_preSkillList.size() != 0)
        {
            for (std::list<_pre_skill>::iterator it =
                     skill->m_preSkillList.begin();
                 it != skill->m_preSkillList.end(); ++it)
            {
                std::map<int, std::vector<std::pair<int, int> > >::iterator
                    found = m_postLearningMap[jobType].find(it->m_index);
                std::pair<int, int> value;
                value.first = skill->get_index();
                value.second = it->m_jobType;
                if (found == m_postLearningMap[jobType].end())
                {
                    std::vector<std::pair<int, int> > vec;
                    vec.push_back(value);
                    // [b28d-solo-CListTables N(P192) DIFF] ORIG 调 value_type 直构
                    // `pair<const int,vector<pair<int,int>>>::C2IRiRS4_EEOT_OT0_`，
                    // 但本 TU 改 value_type 实测 identical -2 / ae -5（他函数符号集
                    // 变化）⇒ 回退保持 make_pair（batch-25c 结论复证）。
                    m_postLearningMap[jobType].insert(
                        std::make_pair(it->m_index, vec));
                }
                else
                {
                    found->second.push_back(value);
                }
            }
        }
    }
