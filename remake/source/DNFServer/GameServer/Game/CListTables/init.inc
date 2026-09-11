// [b28d-solo-CListTables 修复] ORIG 0x08350c26：866/866（gap=0）+ p167 CLEAR(111/111，
// 差集全为 C1/C2·D1/D2 别名）。本轮真修复：①STSkillScript 镜像补 m_needSp(+0x18)/
// m_needSfp(+0x24)（旧实现把 need_sp/need_sfp 循环错指向 +0x44/+0x50 = pre-skill
// 索引表，**读错字段**）；②镜像尾部补 m_vec284/m_290/m_vec294/m_vec2a0（sizeof
// 0x284→0x2ac，ORIG 对象间隔 0x2ac 实证）；③m_stealLevelPenalty/m_stealSPPenalty
// 偏移互换修正（ORIG SetStealLevelPenalty 读 +0x1f4、SetStealSPPenalty 读 +0x1f0）；
// ④m_growType[6] 具名子对象替代裸偏移（ORIG `shl $2; lea; add $0x24; mov -0x364(,%e,4)`）；
// ⑤need_sp/sfp、learned、vec204/210 循环 size 缓存+int 比较、secondGrowType 嵌套 if（P225）、
// m_stealable bool、pre.m_group 删发明写入（P171）、主循环 it 提函数头（ORIG C1Ev 仅一次）、
// 死计数器 -0x78 照抄、push_back(int&&) 临时右值（ORIG 0x0808e1ee 实证）。
// 残差为 N7 寄存器分配/N82 槽位族 ⇒ 语义完成、byte 层封顶。
bool CSkillList::init()  // [P330 int→bool] 与 CDataManager.h 声明同步（ORIG caller xor $1 形）
{
    // [b28d-solo-CListTables] 866/866 + p167 CLEAR(111/111) ⇒ gap=0。
    STSkillScript script;
    CSkill* skill = 0;
    sync_script::GetInstanceSyncScript()->truncate_skill_index();
    sync_script::GetInstanceSyncScript()->trancate_skill_info();
    // [b28d-solo-CListTables] ORIG 0x08350c40/0x08350c61：-0x78 与 path(-0x74) 两处
    // 清零；-0x78 在内层循环尾每轮 +1（0x083517f4）且全函数无读取点 ⇒ 原码遗留死
    // 计数器（P171 照抄死写入）。
    int scriptCount = 0;
    // [b28d-solo-CListTables] ORIG 0x08350cbc 存 c_str() 指针于 -0x74 并 `cmpl $0`
    // ⇒ path 是 char*（旧实现用 int 存指针 + cast，实参装载序与 ORIG 相反）。
    const char* path = 0;
    // [b28d-solo-CListTables P309] ORIG 深区槽序：it(-0xa8) 深于 pre(-0xa4..-0x9c)
    // ⇒ 4.4 函数头声明「先声明浅」，pre 声明在 it 之前即可复现（pre 为 POD，
    // 提头无构造副作用；循环内仅赋值）。
    _pre_skill pre;
    // [b28d-solo-CListTables] ORIG 0x08350c78：主循环 map 迭代器 C1Ev 只在函数头
    // 执行一次（循环外声明，-0xa8）；循环体内仅 begin() 赋值。
    std::map<int, std::string>::iterator it;
    int job;
    for (job = 0; job <= 10; ++job)
    {
        // [b28d-solo-CListTables] ORIG：循环体内 begin() 赋值 + 尾后置 it++（ppEi，P28）。
        it = g_staSkillScriptFileList[job].begin();
        while (it != g_staSkillScriptFileList[job].end())
        {
            path = it->second.c_str();
            if (path == 0)
            {
                break;
            }
            if (!ImportSkillScript(&script, path, (ENUM_CHARACTERJOB)job, 0, -1))
            {
                printf("Skill Script Error: %s\n", path);
                return 0;
            }
            if (!sync_script::GetInstanceSyncScript()
                     ->insert_skill_index(&script, job))
            {
                printf("insert_skill_index Error: %s\n", path);
                return 0;
            }
            if (sync_script::GetInstanceSyncScript()->IsEnable())
            {
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&script, job, 0))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s2;
                // [b28d-solo-CListTables] ORIG 0x08350e20/0x08350e6f：Import 与
                // insert_skill_info 各自独立 printf（两条消息串不同）。
                if (!ImportSkillScript(&s2, path, (ENUM_CHARACTERJOB)job, 4, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s2, job, 4))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s3;
                if (!ImportSkillScript(&s3, path, (ENUM_CHARACTERJOB)job, 6, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s3, job, 6))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s4;
                if (!ImportSkillScript(&s4, path, (ENUM_CHARACTERJOB)job, 11, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s4, job, 11))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
            }
            skill = new CSkill();
            skill->set_index(script.m_index);
            skill->reform_ui_group_no(script.m_group, script.m_field260,
                                      script.m_fitGrowType.size());
            skill->set_group(script.m_group);
            skill->set_job_type(job);
            skill->set_skill_type(script.m_skillType);
            skill->set_level_limit(script.m_levelLimit);
            skill->m_fieldC8 = script.m_field34;
            // [b28d-solo-CListTables] ORIG 0x08351179 `movzbl; movzbl %al` 直传
            // （无 setne）⇒ m_stealable 为 bool 字段、实参直读。
            skill->SetStealable(script.m_stealable);
            skill->SetStealLevelPenalty(script.m_stealLevelPenalty);
            skill->SetStealSPPenalty(script.m_stealSPPenalty);
            for (unsigned int i = 0; i < script.m_fitGrowType.size(); ++i)
            {
                skill->set_fitGrowType(i, script.m_fitGrowType[i]);
            }
            for (int i = 0; i <= 5; ++i)
            {
                skill->set_fitness(i, script.m_fitness[i]);
            }
            for (int i = 0; i <= 5; ++i)
            {
                skill->set_type_max_level(i, script.m_typeMaxLevel[i]);
            }
            // [b28d-solo-CListTables] ORIG 0x08351307 `cmpl $0x1; setbe` ⇒ unsigned。
            for (unsigned int i = 0; i <= 1; ++i)
            {
                // [b28d-solo-CListTables P225 推广] ORIG 两条件各自 `setX; test; je`
                // （0x083512aa/0x083512c7）⇒ 拆嵌套 if，`&&` 短路会被 4.4 折成直跳。
                if (!script.m_secondGrowType.empty())
                {
                    if ((unsigned int)i < script.m_secondGrowType.size())
                    {
                        skill->set_fitness_second_growtype(
                            i + 1, script.m_secondGrowType[i]);
                    }
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_fitness(
                        i, j + 1, script.m_growType[i].m_0[j]);
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_max_level(
                        i, j + 1, script.m_growType[i].m_8[j]);
                }
            }
            // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x083513de/0x0835143b：
            // need_sp 读 script+0x18、need_sfp 读 script+0x24（旧实现错读 +0x44/+0x50）。
            // 循环形：size 先缓存具名局部（-0x6c/-0x68），比较 `setl`（signed）。
            int needSpCount = script.m_needSp.size();
            for (int i = 0; i < needSpCount; ++i)
            {
                skill->add_need_sp(script.m_needSp.at(i));
            }
            int needSfpCount = script.m_needSfp.size();
            for (int i = 0; i < needSfpCount; ++i)
            {
                skill->add_need_sfp(script.m_needSfp.at(i));
            }
            int preSkillCount = script.m_vec44.size();
            for (int i = 0; i < preSkillCount; ++i)
            {
                pre.m_index = script.m_vec44.at(i);
                pre.m_jobType = script.m_vec50.at(i);
                // [P171 照抄] ORIG 0x083514fd 直读栈槽 -0x9c（= pre+0x08）且全函数
                // 无写入点 ⇒ 原码未给 m_group 赋值，不得补写（旧实现多一条 store）。
                skill->add_learned_skill(pre);
            }
            int guildGspCount = script.m_vec204.size();
            for (int i = 0; i < guildGspCount; ++i)
            {
                // [b28d-solo-CListTables] ORIG 0x08111126 = push_back(int const&)
                // （nm 实证）；实参为 at() 返回的 int&（`mov %eax,0x4(%esp)`），
                // 旧实现用 static_cast<int&&> 会多一次解引用+临时槽。
                skill->m_guildSkillNeedGSP.push_back(script.m_vec204.at(i));
            }
            int guildGoldCount = script.m_vec210.size();
            for (int i = 0; i < guildGoldCount; ++i)
            {
                skill->m_guildSkillNeedGold.push_back(script.m_vec210.at(i));
            }
            skill->m_needGuildLevel = script.m_field3c;
            skill->m_needGuildLevelAdd = script.m_field40;
            skill->m_field104 = script.m_field14;
            for (unsigned int i = 0; i < script.m_fitnessTable.size(); ++i)
            {
                std::vector<int> vec;
                for (unsigned int j = 0;
                     j < script.m_fitnessTable[i].size(); ++j)
                {
                    // [b28d-solo-CListTables] ORIG 0x083516a3 调 0x0808e1ee =
                    // vector<int>::push_back(int&&)（nm 实证）：实参是 ROUND 截断的
                    // **临时右值**（先 `mov %eax,-0x94` 再传 &-0x94）⇒ 原码内联
                    // 临时表达式，具名 int 局部会改选 push_back(int const&) 并丢
                    // 3 个弱实例符号（AE 账 -3）。
                    vec.push_back((int)script.m_fitnessTable[i].at(j));
                }
                skill->m_field108.push_back(vec);
            }
            skill->m_isSpecialSkill = script.m_field260;
            skill->m_keyCommandList = script.m_keyCmd;
            // [b28d-solo-CListTables] CSkill.h:136 具名字段（ORIG 0x08351779
            // `movzbl -0xe8; mov %dl,0x130` 单条 disp store）。
            skill->m_isCommandCustomizing = script.m_field280;
            skill->setSkillName(script.m_name.c_str());
            skill->m_pvpSkillGrade = script.m_field38;
            insert_skill(job, skill);
            it++;
            scriptCount++;
        }
    }
    for (int i = 0; i <= 10; ++i)
    {
        // [b28d-solo-CListTables] ORIG：尾部循环迭代器先 C1Ev 后 begin() 赋值
        // （P50）+ 条件区每轮重调 end()（sret -0x8c，无具名 end 缓存）+ 后置
        // ++it（ppEi，P28）。
        __gnu_cxx::hash_map<int, CSkill*>::iterator it;
        it = m_skillList[i].begin();
        while (it != m_skillList[i].end())
        {
            skill = it->second;
            skill->set_prelearned_skill();
            set_post_learning_skill(i, skill);
            it++;
        }
    }
    return 1;
}