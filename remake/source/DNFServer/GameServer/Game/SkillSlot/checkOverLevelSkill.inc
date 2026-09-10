bool SkillSlot::checkOverLevelSkill(std::vector<stSPReturnInfo_t>& out,
                                    ENUM_SKILL_TREE_KIND kind, int level)
{
    // [b28d-solo-SkillSlot] 判词（N82 封顶；608 vs 620，gap=+12）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `std::list<_pre_skill>::end()` EXTRA（循环外 end() 预求值形待核，P13）。
    // 残余为跨版本槽/寄存器/求值序族（大函数 ndiff 失真，见 N82；验收须走
    // §6.2 三件套）。
    if (m_parent == 0 ||
        ((CUserCharacInfo*)m_parent)->getCurCharacR() == 0)
    {
        return 0;
    }
    char result = 0;
    _Mastered_skill* buf = get_skillslot_buf(kind);
    int* remainSFP = 0;
    if (kind == 0)
    {
        remainSFP = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)2);
    }
    else if (kind == 1)
    {
        remainSFP = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)3);
    }
    int* remainSP = get_cur_remain_sp(kind);
    if (buf == 0 || remainSP == 0 || remainSFP == 0)
    {
        return 0;
    }
    int expertJobLevel = 0;
    int expertSkillCount = 0;
    void* expertScript = 0;
    int i;
    CSkill* skill;
    bool isSteal;
    int checkLevel;
    int maxLevel;
    int giveLevel;
    char giveResult;
    bool found;
    unsigned int j;
    int preIndex;
    int preJobType;
    int preGroup;
    expertScript = G_CDataManager()->GetExpertJobScript(
        ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
    if (((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType() != 0)
    {
        expertJobLevel = m_parent->GetCurExpertJobLevel(
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobExp());
    }
    for (i = 0; i < 0xcc; ++i)
    {
        if (buf[i].m_index != 0)
        {
            skill = 0;
            skill = G_CDataManager()->find_skill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (unsigned char)buf[i].m_index);
            if (skill == 0)
            {
                return 0;
            }
            isSteal = false;
            checkLevel = level;
            if (skill->IsStealable(
                    (int)((CUserCharacInfo*)m_parent)
                        ->getCurCharFirstGrowType(),
                    (int)((CUserCharacInfo*)m_parent)
                        ->getCurCharSecondGrowType()) != 0)
            {
                isSteal = true;
                checkLevel = checkLevel - skill->GetStealLevelPenalty();
            }
            checkLevel = checkLevel +
                         m_parent->GetPremiumInfo()->getOverSkillLevel();
            maxLevel = skill->getMaxSkillLevel(
                checkLevel,
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharFirstGrowType(),
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharSecondGrowType());
            if (maxLevel == -1)
            {
                buf[i].m_index = 0;
                buf[i].m_level = 0;
                continue;
            }
            if (expertScript != 0)
            {
                found = false;
                // [round5-A17 2026-09-10] 推翻 [round1-L19]「oracle 视图须为 const」
                // 判词：fresh .o 的 p167 证明 ORIG 侧共 **3 处** 转换构造
                // `__normal_iterator<const pair*>::C2<pair*>`（本循环 begin 一处、
                // giveList 循环 begin/end 各一处）⇒ `.begin()` 必走**非 const**
                // 重载（`_ZNSt6vectorISt4pairIiiESaIS1_EE5beginEv`）才能产生；
                // const 视图反而多发 `_ZNKSt6vector…5beginEv/…5endEv`（p167 EXTRA）
                // 并把比较退化成 const/const ⇒ 维持非 const 成员直访问。
                std::vector<std::pair<int, int> >::const_iterator eit =
                    reinterpret_cast<STExpertJobScript*>(expertScript)
                        ->m_growSkillVec.begin();
                while (eit !=
                       reinterpret_cast<STExpertJobScript*>(expertScript)
                           ->m_growSkillVec.end())
                {
                    if (eit->first == (unsigned char)buf[i].m_index)
                    {
                        if ((char)buf[i].m_level == expertJobLevel)
                        {
                            expertSkillCount = expertSkillCount + 1;
                        }
                        found = true;
                        break;
                    }
                    ++eit;
                }
                if (found)
                {
                    continue;
                }
            }
            std::list<_pre_skill>::const_iterator pit;
            const std::list<_pre_skill>& preSkillList = skill->m_preSkillList;
            pit = preSkillList.begin();
            while (pit != preSkillList.end())
            {
                const _pre_skill& preSkill = *pit;
                preIndex = preSkill.m_index;
                preJobType = preSkill.m_jobType;
                preGroup = preSkill.m_group;
                if (check_skill_mastered(preIndex, preJobType, preGroup,
                                         kind) != 1)
                {
                    maxLevel = 0;
                    break;
                }
                pit++;
            }
            if (maxLevel < (char)buf[i].m_level)
            {
                giveLevel = 0;
                std::vector<std::pair<int, int> > giveList(
                    *(std::vector<std::pair<int, int> >*)(
                        (char*)G_CDataManager()->m_characList +
                        ((CUserCharacInfo*)m_parent)->get_charac_job() *
                            0x7dc +
                        0x240));
                giveResult = 1;
                if (((CUserCharacInfo*)m_parent)
                        ->getCurCharSecondGrowType() == 0)
                {
                    giveResult = ((CCharacter*)((char*)G_CDataManager()
                                                    ->m_characList +
                                                ((CUserCharacInfo*)m_parent)
                                                        ->get_charac_job() *
                                                    0x7dc))
                                     ->get_give_skill(
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharFirstGrowType(),
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharSecondGrowType(),
                                         giveList, 1);
                }
                else
                {
                    giveResult = ((CCharacter*)((char*)G_CDataManager()
                                                    ->m_characList +
                                                ((CUserCharacInfo*)m_parent)
                                                        ->get_charac_job() *
                                                    0x7dc))
                                     ->get_give_skill(
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharFirstGrowType(),
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharSecondGrowType(),
                                         giveList, 0);
                }
                if (giveResult == 1)
                {
                    giveLevel = 0;
                    std::vector<std::pair<int, int> >::const_iterator git =
                        giveList.begin();
                    std::vector<std::pair<int, int> >::const_iterator gitEnd =
                        giveList.end();
                    while (git != gitEnd)
                    {
                        if (git->first == (unsigned char)buf[i].m_index)
                        {
                            giveLevel = git->second;
                            break;
                        }
                        ++git;
                    }
                    maxLevel = maxLevel <= giveLevel ? giveLevel : maxLevel;
                    if (maxLevel < (char)buf[i].m_level)
                    {
                        int retSP = 0;
                        int retSFP = 0;
                        calcReturnSpSfpPerOneSkill(
                            retSP, retSFP, maxLevel, buf, i, skill,
                            isSteal);
                        out.push_back(stSPReturnInfo_t(
                            (unsigned short)(unsigned char)buf[i].m_index,
                            (unsigned short)((short)(char)buf[i].m_level -
                                             (short)maxLevel),
                            (unsigned int)retSP, (unsigned int)retSFP));
                        *remainSFP = *remainSFP + retSFP;
                        *remainSP = *remainSP + retSP;
                        buf[i].m_level = (char)maxLevel;
                        if (buf[i].m_level == 0)
                        {
                            buf[i].m_index = 0;
                        }
                        result = 1;
                    }
                }
            }
        }
    }
    if (expertScript != 0 &&
        (*(std::vector<std::pair<int, int> >*)((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                .size() != expertSkillCount)
    {
        for (j = 0;
             j < (*(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                        0xc))
                     .size();
             ++j)
        {
            growtype_skill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (*(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                       0xc))[j]
                    .first,
                expertJobLevel, kind);
            result = 1;
        }
    }
    return result;
