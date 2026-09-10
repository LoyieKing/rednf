bool StageMapList::loadList()
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（§6.2 口径，残余帧/槽差；p167 CLEAR 55=55）。
    //   已修：三层遍历由索引 size()/operator[] 改迭代器（begin/end/operator!=/
    //   operator->/operator++(int)，P28 后置 ++）+ ORIG 的容器引用缓存（Ghidra C
    //   `local_20 = local_20 + 0x20`）+ 尾循环条件/体内各调 summaries.size()
    //   （ORIG 三处调用点）⇒ 本件 +18 AE。
    //   [本波实测·三形均未翻正，判 N] ORIG 0x08364875 `movl $0x0,-0x28(%ebp)`
    //   （预清）→ 0x083648b3 `mov %eax,-0x28`（= it->second.c_str()）→ 0x083648b6
    //   `cmpl $0x0,-0x28; je`（判空 break）⇒ 原码是函数级具名 `const char* name`。
    //   实测三形：(a) 循环内 `const char* name = c_str()`（现状）gap 3 / ndiff 359；
    //   (b) 补 `int ok = 0;` 两句式 gap 1 / ndiff 57；(c) 函数级
    //   `const char* name = 0;`+循环内赋值 gap 2 / ndiff 360。三形均为 DIFF（分类
    //   不变），且 (c) 反而劣化 ⇒ 帧差（我方 0xe0 vs ORIG 0xf0，判据⑪ 整帧平移，
    //   N126）非声明序可修；恢复语义最干净形。ORIG 0x0836485c，p167 CLEAR 55=55。
    std::vector<_StageMapSummary> summaries;

    for (std::map<int, std::string>::iterator it =
             g_stageMapScriptFileList.m_fileMap.begin();
         it != g_stageMapScriptFileList.m_fileMap.end(); it++)
    {
        const char* name = it->second.c_str();
        if (name == 0)
        {
            break;
        }

        stagemap::StageMapScript script;
        if (!importStageMapScript(&script, name))
        {
            printf("Stage Map Script Error: %s\n", name);
            return false;
        }

        // p167：ORIG 三层均为迭代器遍历（begin/end/operator!=/operator->/
        // operator++(int)），非索引 + size()/operator[]；后置 ++ 由 P28 增量侧
        // 推论（ORIG 增量段调 ppEi）。
        for (std::vector<stagemap::AreaData>::iterator ai =
                 script.m_areaVec.begin();
             ai != script.m_areaVec.end(); ai++)
        {
            int districtId = 0;
            // ORIG 缓存容器引用（Ghidra C：`local_20 = local_20 + 0x20` 即 &ai->m_vec20
            // 一次取址；p167 显示我方 begin/end 各多一次 operator->）。
            std::vector<stagemap::DistrictData>& districts = ai->m_vec20;
            for (std::vector<stagemap::DistrictData>::iterator di =
                     districts.begin();
                 di != districts.end(); di++)
            {
                int stageIdx = 0;
                std::vector<stagemap::StageData>& stages = di->m_vec4;
                for (std::vector<stagemap::StageData>::iterator si =
                         stages.begin();
                     si != stages.end(); si++)
                {
                    _StageMapSummary sum;
                    sum.m_nextId = 0;
                    sum.m_stageId = si->m_field0;
                    sum.m_uniqueId =
                        makeStageMapUniqueId(ai->m_field0, di->m_field0,
                                             stageIdx);
                    summaries.push_back(sum);
                    ++stageIdx;
                }
                districtId = di->m_field0;
            }
            districtId += 1;
            int stageIdx2 = 0;
            if (0 < ai->m_field2c)
            {
                _StageMapSummary sum;
                sum.m_nextId = 1;
                sum.m_stageId = ai->m_field2c;
                sum.m_uniqueId =
                    makeStageMapUniqueId(ai->m_field0, districtId, stageIdx2);
                summaries.push_back(sum);
            }
        }
    }

    std::sort(summaries.begin(), summaries.end(), compare);

    // ORIG 0x08365… 尾循环条件每轮调 size()，体内另调一次（p167 MISSING size×1；
    // 报告 Ghidra C 556-591 三处 size() 调用点）。
    for (size_t i = 0; i < summaries.size(); ++i)
    {
        if (i == 0)
        {
            m_firstStageId = summaries[0].m_stageId;
        }
        if (summaries.size() - 1 != i)
        {
            summaries[i].m_nextId = summaries[i + 1].m_stageId;
        }
        m_map.insert(std::make_pair(summaries[i].m_stageId, summaries[i]));
    }
    return true;
