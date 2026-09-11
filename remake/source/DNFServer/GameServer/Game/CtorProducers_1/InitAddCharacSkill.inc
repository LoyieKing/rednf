bool AvatarRouletteServer::InitAddCharacSkill()
{
    // [b28d-sweepC 判词·P171 真缺口修复 + N82 残余] ORIG 0x0817f386：
    // 本轮修 `outer.size()` 死调用 + 缓存上界两段形（同 Init）。
    // 【真缺口】ORIG 0x0817f5ec `mov -0xc(%ebp),%eax; mov %eax,-0x60(%ebp)` =
    // push_back 前 `skill.m_field4 = idx`（Ghidra C local_64 = local_10）——旧实现
    // 漏写此覆盖（P171 死写入照抄：前面 `var.m_skill.m_field4` 的值被覆盖）。
    // gap 4→2（剩余：4.1 消解引用 var 不落槽 vs 4.4 落槽 4 条 + ORIG key 引用
    // 临时 2 条，方向相反不可同时闭合）。
    // 残余：槽号整体平移（4.1 先声明更深 vs 4.4 首次使用序，N82）⇒ 封顶。
    // ORIG 0x0817f386。
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {

    for (int job = 0; job < 0xb; ++job)
    {
        std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
            m_pRoulette->m_avatars[job];
        // 同 Init：ORIG 0x0817f3d5 `size()` 死调用（结果丢弃，P171）+ `li=0` +
        // 0x0817f3e7 第二次 `size()` 存 -0x20 作循环上界（p167 MISSING×1）。
        outer.size();
        // [轴 B 已试·按 ORIG 直证回滚] li/n 循环的 ORIG 尾测在 **0x0817f6d0-0x0817f6d6**
        // （`mov -0x24(%ebp),%eax; cmp -0x20(%ebp),%eax; setb %al`）= **无符号序**
        // ⇒ li/n 保持 size_t（我方 setcc 实测 = setb ✓）。先前误把 **idx/count 循环**
        // （0x0817f619-0x0817f623，`setl`）当作本循环，改成 int 后本循环变 setl 反而
        // 与 ORIG 相反，已回滚（改前=改后=241/235、TU 四数不变）。
        size_t li = 0;
        size_t n = outer.size();
        for (; li < n; ++li)
        {
            std::vector<std::pair<unsigned long, int> >& inner = outer[li];
            for (int type = 0; type < 9; ++type)
            {
                // [BigUserLogic] ORIG 0x0817f45x 布局 = 正向块 `if (type==3 &&
                // (job==9||job==0)) { … if (fileName) { … } }`（假条件 je 跳 loop
                // 尾，无独立 continue 岛）。旧 `if(!(…)) continue` 反判形产额外
                // jmp/条件翻转。正向化，语义零改动。
                if (type == 3 && (job == 9 || job == 0))
                {
                    const char* fileName =
                        g_stEquipmentScriptFileList.GetFileName(
                            static_cast<int>(inner[type].first));
                    if (fileName != 0)
                    {
                        STEquipmentScript eq;
                        if (!ImportEquipmentScript(&eq, fileName, false, 0, -1))
                        {
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                __PRETTY_FUNCTION__, 0x39,
                                "InitAddCharacSkill() - ImportEquipmentScript() fail file - %s",
                                fileName);
                            return false;
                        }
                        std::map<int, std::vector<LevelUpSkill> > skillMap;
                        int count = static_cast<int>(eq.m_param1c0.m_avatar264.size());
                        for (int idx = 0; idx < count; ++idx)
                        {
                            LevelUpSkill skill;
                            // [判据⑦/P0] ORIG 0x0817f531 只发**一次** operator[]
                            // （0x0818084a）后连读 +0xc/+0x10/+0x14 落在 skill 三槽
                            // ⇒ 原码是子对象**整体赋值**（隐式 operator= 逐字段 3 mov），
                            // 非逐字段引用形：具名引用 `var` 会多占槽并每字段重载 eax
                            // （旧形：-0x4 槽 + `mov -0xc,%eax` 三连）。
                            skill = eq.m_param1c0.m_avatar264[idx].m_skill;
                            // ORIG：先取 end() 再 find(job)（0x0817f548/0x0817f577），
                            // find==end 时先插入空 vector，再 push_back。
                            std::map<int, std::vector<LevelUpSkill> >::iterator itEnd =
                                skillMap.end();
                            std::map<int, std::vector<LevelUpSkill> >::iterator it =
                                skillMap.find(skill.m_job);
                            if (it == itEnd)
                            {
                                std::vector<LevelUpSkill> empty;
                                skillMap[skill.m_job] = empty;
                            }
                            // ORIG 0x0817f5ec：push_back 前把 skill+0x04 覆盖为循环下标
                            // （`mov -0xc(%ebp),%eax; mov %eax,-0x60(%ebp)`；Ghidra C
                            //  local_64 = local_10）。P171 死写入照抄：前一条
                            // `skill.m_field4 = var.m_skill.m_field4` 的值被本行覆盖。
                            skill.m_field4 = idx;
                            skillMap[skill.m_job].push_back(skill);
                        }
                        m_addCharacSkillMap[inner[type].first] = skillMap;
                    }
                }
            }
        }
    }
    }
    return true;
}