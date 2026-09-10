void SkillSlot::calcUsedSP(const _Mastered_skill* buf, int& sp, int& sfp) const
{
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；523 vs 556，gap=+33）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `__gnu_cxx::operator==<pair const*, vector<pair>>` EXTRA（迭代器比较形，
    // P96/P220 非 const iterator + 正极性 `!=` 待核）。
    // 残余为跨版本槽/寄存器族。
    // [round5-A17 2026-09-10] 两处**真实调用点差**已修（fresh 复验）：
    //   ① `eit` 段原写「预求值 eend + `==`」⇒ p167 EXTRA `__gnu_cxx::eq<const pair*,
    //      vector>`（ORIG 为混合 `ne<const pair*, pair*>`）⇒ 改回
    //      `while (eit != …end())`（end() 走非 const ⇒ iterator，与 const_iterator 混合）；
    //   ② 函数头 `CCharacter* character = …get_charac_job()…` 具名缓存 + 两分支内二次
    //      调用使 `get_charac_job` 比 ORIG **多 1 次**（p167 EXTRA）⇒ 删具名 cache、
    //      两分支 receiver 内联。
    // 读数：552/528 → **520 vs 535（gap=-15）**、ndiff 728→619；p167 calls 77=77
    // CLEAR（余 C1↔C2/D1↔D2 同址别名，nm 实证 ORIG 0x081349d6 / 0x081349ea）。
    int expertJobLevel = 0;
    int expertSkillCount = 0;
    int expertScript = 0;
    if (((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType() != 0)
    {
        int exp =
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobExp();
        expertJobLevel = m_parent->GetCurExpertJobLevel(exp);
    }
    // [g11-slot 2026-09-10] P0/P183 去具名缓存（直证）：ORIG 4 处使用点均为
    // `call G_CDataManager; mov 0x14(%eax),%ebx/%edi`（0x08605644/0x08605649、
    // 0x086056bb/0x086056c0、0x0860571d/0x08605722）与
    // `call; lea 0xa800(%eax),%edx`（0x08605962/0x08605967）——每次重发地址链、
    // 无栈槽；旧形 `dm` 具名占槽（我方 -0x50 的 store+load 往返）。
    std::vector<std::pair<int, int> > giveList(
        *(std::vector<std::pair<int, int> >*)(
            (char*)G_CDataManager()->m_characList +
            ((CUserCharacInfo*)m_parent)->get_charac_job() * 0x7dc + 0x240));
    std::vector<std::pair<int, int> > learnList;
    // 2026-09-10（D06 消费侧配对 bool 化，A17 spec）：CCharacter::get_give_skill
    //   定义侧已 char→bool ⇒ 本处 result 同步 bool（ORIG 0x086056xx 为
    //   `mov %al,-0x35; movzbl -0x35; xor $1; test; je` = bool 负极性物化）。
    bool result = true;
    char secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
    char firstGrow = 0;
    if (secondGrow == 0)
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        result = ((CCharacter*)((char*)G_CDataManager()->m_characList +
                                ((CUserCharacInfo*)m_parent)
                                    ->get_charac_job() *
                                    0x7dc))
                     ->get_give_skill((int)firstGrow, (int)secondGrow,
                                      learnList, 1);
    }
    else
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        result = ((CCharacter*)((char*)G_CDataManager()->m_characList +
                                ((CUserCharacInfo*)m_parent)
                                    ->get_charac_job() *
                                    0x7dc))
                     ->get_give_skill((int)firstGrow, (int)secondGrow,
                                      learnList, 0);
    }
    // 2026-09-10 试形留痕（A17 极性直证 + 我方实测）：按 ORIG 0x0860575d
    //   `movzbl -0x35; xor $1; test; je <主体>` 改早退形（`if (!result) return;`
    //   + 主体裸块）⇒ ours 538→**544**（劣 6 条，ORIG 523，gap −15→−21）⇒ 已回滚。
    //   bool 配对化（`bool result`，D06 消费侧）保留：四数 64/184/0/11 不降。
    if (result)
    {
        std::vector<std::pair<int, int> >::const_iterator git =
            learnList.begin();
        std::vector<std::pair<int, int> >::const_iterator gend =
            learnList.end();
        while (git != gend)
        {
            std::vector<std::pair<int, int> >::iterator lit =
                giveList.begin();
            std::vector<std::pair<int, int> >::iterator lend =
                giveList.end();
            while (lit != lend)
            {
                if (lit->first == git->first)
                {
                    lit->second = std::max(lit->second, git->second);
                    break;
                }
                ++lit;
            }
            if (lit == lend)
            {
                giveList.push_back(std::make_pair(git->first, git->second));
            }
            ++git;
        }
        GameWorld* world = G_GameWorld();
        if (world->IsPvPSkilTreeChannel() != 0)
        {
            giveList.clear();
            secondGrow =
                ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
            char firstGrow =
                ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
            int job = ((CUserCharacInfo*)m_parent)->get_charac_job();
            (&G_CDataManager()->m_pvpSkillTree)
                ->getGiveSkill(job, (int)firstGrow, (int)secondGrow,
                               giveList);
        }
        for (int i = 0; i < 204; ++i)
        {
            if (buf[i].m_index != 0)
            {
                CSkill* skill = 0;
                int job = ((CUserCharacInfo*)m_parent)->get_charac_job();
                skill = G_CDataManager()->find_skill(job,
                                                     (unsigned char)buf[i].m_index);
                if (skill == 0)
                {
                    break;
                }
                bool isSteal = false;
                int charLevel =
                    ((CUserCharacInfo*)m_parent)->get_charac_level();
                secondGrow =
                    ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
                firstGrow =
                    ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
                if (skill->IsStealable((int)firstGrow, (int)secondGrow) != 0)
                {
                    isSteal = true;
                    charLevel = charLevel - skill->GetStealLevelPenalty();
                }
                charLevel = charLevel +
                            (unsigned short)m_parent->GetPremiumInfo()
                                ->getOverSkillLevel();
                int giveLevel = 0;
                std::vector<std::pair<int, int> >::const_iterator lit =
                    giveList.begin();
                std::vector<std::pair<int, int> >::const_iterator lend =
                    giveList.end();
                while (lit != lend)
                {
                    if (lit->first == (unsigned char)buf[i].m_index)
                    {
                        giveLevel = lit->second;
                        break;
                    }
                    ++lit;
                }
                int curLevel = 0;
                if (giveLevel != 0)
                {
                    curLevel = giveLevel;
                    if (giveLevel < 0)
                    {
                        curLevel = 0;
                    }
                    if (buf[i].m_level <= curLevel)
                    {
                        continue;
                    }
                }
                if (expertScript != 0)
                {
                    bool found = false;
                    std::vector<std::pair<int, int> >::const_iterator eit =
                        (*(std::vector<std::pair<int, int> >*)
                             ((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                            .begin();
                    while (eit !=
                           (*(std::vector<std::pair<int, int> >*)
                                ((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                               .end())
                    {
                        if (eit->first == (unsigned char)buf[i].m_index)
                        {
                            if (buf[i].m_level == expertJobLevel)
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
                int retSP = 0;
                int retSFP = 0;
                calcReturnSpSfpPerOneSkill(retSP, retSFP, curLevel, buf, i,
                                           skill, isSteal);
                sp = sp + retSP;
                sfp = sfp + retSFP;
            }
        }
    }
