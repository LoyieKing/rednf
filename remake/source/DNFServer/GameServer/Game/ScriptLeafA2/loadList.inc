// ============================================================================
// StageMapList::loadList —— ORIG T 0x0836485c
// 报告 StageMapList/loadList.md 全文转写。
// ============================================================================
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
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N126）→ 试=④P339/P232：按 ORIG 帧 0xf0
    //   （我方 0xe0）补函数级 `int pad;` ⇒ 帧对齐 0xf0，但 name 槽仍差 4（ORIG -0x28
    //   vs 我方 -0x24）且 4.4 对未初始化 int 恒发 `movl $0x0,-0x24`（+1 条）⇒ 槽号不可控，
    //   已回滚（356/356 gap 0 与原状同）。⇒ 维持 N126。
    std::vector<_StageMapSummary> summaries;
    // [b28d-L4a 2026-09-11] ORIG 0x08364875 `movl $0x0,-0x28(%ebp)`（summaries
    // ctor 之后、begin() 之前）⇒ 函数级具名 `name` 带初值 0，循环内 `mov %eax,-0x28`
    // 覆写（形 (c) 复原）。
    const char* name = 0;

    for (std::map<int, std::string>::iterator it =
             g_stageMapScriptFileList.m_fileMap.begin();
         it != g_stageMapScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
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
                    // [b28d-L4a 2026-09-11] 真语义修复（字段错位轴③）：ORIG
                    // 0x083649ad `movb $0x0,-0xa0(%ebp)` = sum+0xc（1 字节），
                    // 0x08364b09 `movb $0x1,-0xb0(%ebp)` = sum+0xc（幕区收尾）；
                    // ORIG 构造段对 +0x08（int m_nextId，尾循环 0x08364cec 起
                    // `mov 0x4(%eax),%eax; mov %eax,0x8(%ebx)` 写入）**只靠 C1
                    // memset 清零**。我方旧式 `sum.m_nextId = 0/1`（movl 写 +8）
                    // 使 +0xc 的 bool 永远为 0 ⇒ getNextStage 0x08364e58 读该字节
                    // 时 ORIG=1 / 我方=0，行为分叉。
                    sum.m_flags = 0;
                    sum.m_stageId = si->m_field0;
                    sum.m_uniqueId =
                        makeStageMapUniqueId(ai->m_field0, di->m_field0,
                                             stageIdx);
                    summaries.push_back(sum);
                    ++stageIdx;
                }
                districtId = di->m_field0;
            }
            // ORIG 0x08364ad6 `movl $0x0,-0x18(%ebp)`（stageIdx2 预清）**先于**
            // 0x08364add `addl $0x1,-0x20(%ebp)`（districtId += 1）⇒ 语句序复原。
            int stageIdx2 = 0;
            districtId += 1;
            if (0 < ai->m_field2c)
            {
                _StageMapSummary sum;
                sum.m_flags = 1;      // ORIG 0x08364b09 `movb $0x1,-0xb0(%ebp)`
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
            // ORIG 0x08364cb4 `mov -0xc(%ebp),%eax; mov %eax,0x4(%esp)`（传**变量 i**
            // 而非立即数 0）⇒ 原码 `summaries[i]`（i==0 域内等价）。
            m_firstStageId = summaries[i].m_stageId;
        }
        if (summaries.size() - 1 != i)
        {
            summaries[i].m_nextId = summaries[i + 1].m_stageId;
        }
        m_map.insert(std::make_pair(summaries[i].m_stageId, summaries[i]));
    }
    return true;
}